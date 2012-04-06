// Copyright (c) 2011 Sirikata Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef _SIRIKATA_SDLAUDIO_SDLAUDIO_HPP_
#define _SIRIKATA_SDLAUDIO_SDLAUDIO_HPP_

#include <sirikata/oh/Simulation.hpp>
#include <sirikata/core/service/Context.hpp>
#include <sirikata/core/transfer/TransferMediator.hpp>
#include <sirikata/core/transfer/ResourceDownloadTask.hpp>
#include <sirikata/core/util/Liveness.hpp>

#include "SoundCommon.hpp"
#include "URLFullSoundLoader.hpp"
#include "FFmpegAudioStream.hpp"

namespace Sirikata {
namespace SDL {

#define CLIP_HANDLE_NULL  0


class AudioSimulation : public Simulation,
                        public Liveness

{
public:
    AudioSimulation(Context* ctx, Network::IOStrandPtr aStrand, const SpaceObjectReference& sporef);
    virtual ~AudioSimulation();

    // Service Interface
    virtual void start();
    virtual void stop();

    // Invokable Interface
    virtual boost::any invoke(std::vector<boost::any>& params);

    // Mixing interface, public for the mixing callback function
    void mix(uint8* raw_stream, int32 len);

private:
    void iStart(Liveness::Token lt);
    void iStop(Liveness::Token lt);

    bool initialized;

    // Indicates whether basic initialization was successful, i.e. whether we're
    // going to be able to do any operations.
    bool ready() const;

    void handleFinishedDownload(
        Transfer::ResourceDownloadTaskPtr taskptr,
        Transfer::TransferRequestPtr request,
        Transfer::DenseDataPtr response);

    void iHandleFinishedDownload(
        Liveness::Token lt,
        Transfer::ResourceDownloadTaskPtr taskptr,
        Transfer::TransferRequestPtr request,
        Transfer::DenseDataPtr response);

    Network::IOStrandPtr audioStrand;


    Context* mContext;
    bool mInitializedAudio;
    bool mOpenedAudio;

    SpaceObjectReference mSporef;

    //FIXME:
    //make this an atomic int?
    ClipHandle mClipHandleSource;

    
    // Mixing callbacks come from SDL's thread, download handlers come from
    // transfer thread, so we need to be careful about managing access.
    Mutex mMutex;

    Transfer::TransferPoolPtr mTransferPool;
    struct DownloadTask {
        Transfer::ResourceDownloadTaskPtr task;
        std::set<ClipHandle> waiting;
    };
    typedef std::map<Transfer::URI, DownloadTask> DownloadTaskMap;
    DownloadTaskMap mDownloads;

    // ClipHandles are used to uniquely identify playing clips
    ClipMap mClips;

    bool mPlaying;

    //handling invokable input functions

    URLFullSoundLoaderManager* mURLFullSoundLoaderManager;
    
    /**
       If get a request to download full audio, and then play it, downloads file
       from URL and then issues callback to add file to streams to play.
     */
    boost::any startDownloadAndPlayFromURL(std::vector<boost::any>& params);

    /**
       Called from within audiostrand.  Starts playing file that had been
       downloaded.
     */
    void playDownloadFinished(
        FullSoundLoaderStatus status,Transfer::DenseDataPtr response,
        std::set<ClipHandle> waitingClips,Liveness::Token lt);

    

    //insures that mClipHandleSource will never return clip handle null
    ClipHandle incrementClipId();
        
};



} //namespace SDL
} //namespace Sirikata

#endif //_SIRIKATA_SDLAUDIO_SDLAUDIO_HPP_
