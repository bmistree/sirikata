/*  Sirikata Transfer -- Content Distribution Network
 *  EventTransferManager.hpp
 *
 *  Copyright (c) 2009, Patrick Reiter Horn
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are
 *  met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Sirikata nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*  Created on: Feb 21, 2009 */

#ifndef SIRIKATA_EventTransferManager_HPP__
#define SIRIKATA_EventTransferManager_HPP__

#include "task/EventManager.hpp"
#include "CacheLayer.hpp"
#include "NameLookupManager.hpp"
#include "TransferManager.hpp"
#include "util/AtomicTypes.hpp"

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace Sirikata {
namespace Transfer {


class EventTransferManager : public TransferManager {

	CacheLayer *mFirstTransferLayer;
	NameLookupManager *mNameLookup;
	Task::GenEventManager *mEventSystem;
	volatile bool mCleanup;
	AtomicValue<int> mPendingCleanup;
	boost::condition_variable mCleanupCV;

	typedef std::tr1::unordered_multimap<Fingerprint, Range, Fingerprint::Hasher> DownloadRangeMap;
	DownloadRangeMap mActiveTransfers;

	boost::mutex mMutex;

	void downloadFinished(const RemoteFileId &remoteid, const Range &range, const SparseData *downloadedData) {
		bool found = true;
		{
			boost::unique_lock<boost::mutex> l(mMutex);
			DownloadRangeMap::const_iterator iter =
				mActiveTransfers.find(remoteid.fingerprint());
			while (iter != mActiveTransfers.end() && (*iter).first == remoteid.fingerprint()) {
				if (downloadedData ?
						((*iter).second.isContainedBy(*downloadedData)) :
						((*iter).second == range)) {
					// Return value is the iterator immediately following q prior to the erasure.
					iter = mActiveTransfers.erase(iter);
					found = true;
				} else {
					++iter;
				}
			}
		}

		if (found) {
			Status stat;
			if (downloadedData) {
				stat = SUCCESS;
			} else {
				stat = FAIL_DOWNLOAD;
			}
			mEventSystem->fire(DownloadEventPtr(new DownloadEvent(stat, remoteid, downloadedData)));
		} else {
			std::cerr << "Finished download for " << remoteid.uri() << " but event has already fired..." << std::endl;
		}
	}

	void downloadNameLookupSuccess(const EventListener &listener, const Range &range, const RemoteFileId *remoteid) {
		if (!remoteid) {
			listener(DownloadEventPtr(new DownloadEvent(FAIL_NAMELOOKUP, RemoteFileId(), NULL)));
		} else {
			boost::unique_lock<boost::mutex> l(mMutex);

			if (mCleanup) {
				listener(DownloadEventPtr(new DownloadEvent(FAIL_SHUTDOWN, *remoteid, NULL)));
				if (--mPendingCleanup == 0) {
					if (mCleanup) {
						mCleanupCV.notify_one(); // We are the last one to finish.
					}
				}
				return;
			}

			DownloadRangeMap::const_iterator iter =
				mActiveTransfers.find(remoteid->fingerprint());
			bool found = false;
			while (iter != mActiveTransfers.end() && (*iter).first == remoteid->fingerprint()) {
				if (range.isContainedBy((*iter).second)) {
					std::cout << "ISContained " << range << " " << (*iter).second << std::endl;
					found = true;
					break;
				}
				++iter;
			}
			std::cout << "Getting " << range << " (found = " << found << ")" << std::endl;
			mEventSystem->subscribe(DownloadEvent::getIdPair(*remoteid), listener);
			if (!found) {
				mActiveTransfers.insert(
					DownloadRangeMap::value_type(remoteid->fingerprint(), range));
				CacheLayer * theCacheLayer = mFirstTransferLayer;
				// release lock after subscribing to ensure that event does not fire until now.
				l.unlock();

				/* Don't want to own a lock here, but also need to make sure
				 * nobody deletes mFirstTransferLayer while we are in the call.
				 * For any asynchronous callbacks, CacheLayer will handle cleanup,
				 * but for synchronous callbacks it is our responsibility.
				 */

				// FIXME: mFirstTransferLayer may be destroyed if cleanup is called after previous check.
				theCacheLayer->getData(*remoteid, range,
					std::tr1::bind(&EventTransferManager::downloadFinished, this, *remoteid, range, _1));

			}

		}

		if (--mPendingCleanup == 0) {
			if (mCleanup) {
				mCleanupCV.notify_one(); // We are the last one to finish.
			}
		}
	}

	/*
	struct RequestInfo {
		bool mPending; //pending overlapping requests.

		Range mReqRange;
		std::tr1::function<void(const Fingerprint&, const SparseData*)> mCallback;
	};
	typedef std::map<Fingerprint, std::list<RequestInfo> > RequestMap;
	RequestMap mActiveRequests;
	 */
	//boost::mutex mLock;
public:

	EventTransferManager(CacheLayer *download, NameLookupManager *nameLookup, Task::GenEventManager *eventSystem)
			: mFirstTransferLayer(download), mNameLookup(nameLookup), mEventSystem(eventSystem),
			mCleanup(false), mPendingCleanup(0) {
	}

	virtual void cleanup() {
		{
			boost::unique_lock<boost::mutex> cleanuplock(mMutex);

			// mEventSystem is still used to send out last-minute notifications of aborted transfers.
			// But delete the rest:
			mCleanup = true;
			mNameLookup = NULL;
			mFirstTransferLayer = NULL;

			while (mPendingCleanup.read() != 0) {
				// Wait for any downloadNameLookupSuccess callbacks to return.
				mCleanupCV.wait(cleanuplock);
			}
		}

	}

	virtual ~EventTransferManager() {
		boost::unique_lock<boost::mutex> cleanuplock(mMutex);
		//mEventSystem->fire(DownloadEventPtr(new DownloadEvent(FAIL_SHUTDOWN, RemoteFileId(), NULL)));
	}

	virtual void purgeFromCache(const Fingerprint &fprint) {
		mFirstTransferLayer->purgeFromCache(fprint);
	}

	virtual void download(const URI &name, const EventListener &listener, const Range &range) {
		// TODO: Handle multiple name lookups at the same time to the same filename. Is this possible? worth doing?
		++mPendingCleanup;
		mNameLookup->lookupHash(name, std::tr1::bind(&EventTransferManager::downloadNameLookupSuccess, this, listener, range, _1));
	}

	virtual void downloadByHash(const RemoteFileId &name, const EventListener &listener, const Range &range) {
		// This is the same as if the download() function got a cached name lookup response.
		++mPendingCleanup;
		downloadNameLookupSuccess(listener, range, &name);
	}

	virtual bool upload(const URI &name, const SparseData &toUpload) {
		if (!Range(true).isContainedBy(toUpload)) {
			// must upload a whole file.
			return false;
		}
		/*
		Fingerprint fprint (SHA256::computeFromSparseData(toUpload));

		// first check existence of content, then upload content, then upload name.

		ProtocolRegistry<HTTPUpload> * uploadHTTP;
		ProtocolRegistry<NameUpload> * uploadManager;

		return true;
		*/
		return false;
	}
};

}
}

#endif /* SIRIKATA_EventTransferManager_HPP__ */
