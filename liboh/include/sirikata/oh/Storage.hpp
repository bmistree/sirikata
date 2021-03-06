/*  Sirikata
 *  Storage.hpp
 *
 *  Copyright (c) 2010, Stanford University
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

#ifndef __SIRIKATA_OH_STORAGE_HPP__
#define __SIRIKATA_OH_STORAGE_HPP__

#include <sirikata/core/util/Platform.hpp>
#include <sirikata/oh/Platform.hpp>
#include <sirikata/oh/HostedObject.hpp>
#include <sirikata/core/util/Factory.hpp>

namespace Sirikata {
namespace OH {

/** Represents a backing store for persistent object storage. Each
 *  object is assigned an identifier which maps to a 'bucket' of
 *  values in the backing store, keeping each object isolated. Within
 *  that bucket, objects can write to keys, specified as strings.
 *
 *  Storage implementations *must* support transactions for atomic
 *  commits to multiple keys. There are two modes - transactions and
 *  single-key operations. In transactional mode, you call
 *  beginTransaction(), call other methods to add operations to the
 *  transaction, and finally call commit() to run the
 *  transaction. Single-key operations are called without any
 *  surrounding invokations of transaction methods. The implementation
 *  can send them to be performed immediately.  Single-key operations
 *  are really transactions, but elide the beginTransaction() and
 *  commit() calls.
 *
 *  Ordering of transactions (and single-key transactions) are not
 *  guaranteed: if you need to ensure operations happen in order you
 *  should either batch them in a transaction or wait for the callback
 *  for each one before processing the next.
 *
 *  Implementations may use locks or leases to implement
 *  transactions. In that case, it is possible that they fail to
 *  execute a transaction because the database is currently
 *  locked. Users are responsible for resolving these problems by
 *  retrying or dropping the transaction. Implementations can expose
 *  control over leases and locks via their settings so they can,
 *  e.g., ensure only one OH accesses a storage bucket at once.
 */
class SIRIKATA_OH_EXPORT Storage : public Service {
public:
    // A storage implementation contains a set of 'buckets' which
    // provide isolated storage for each object.
    typedef UUID Bucket;

    // Keys are arbitrary-length strings -- the namespace is flat, but
    // it is easy to add human readable hierarchy using a separator
    // (e.g. '.') and a bit of escaping.
    typedef String Key;

    /** Small number of results. These indicate a few standard error
     *  conditions or success.
     */
    enum Result {
        // Successful. Any data read back should be available in
        // corresponding callback arguments.
        SUCCESS = 0,

        // Failed to lock database or gain a lease. This rarely
        // indicates permanent failure. Rather, it indicates that
        // access to the database was blocked, probably temporarily as
        // an existing lease is blocking it (possibly to be freed up
        // when it automatically expires). Retrying is a reasonable
        // course of action here.
        LOCK_ERROR = 1,

        // There was an error applying the transaction. This could be due to a
        // missing key (for reads), a compare error, or and error writing or
        // erasing a key. The particular error is not specified because
        // operations can be reordered by the implementation for efficiency.
        TRANSACTION_ERROR = 2
    };

    typedef std::tr1::unordered_map<Key, String> ReadSet;
    /** CommitCallbacks are invoked when a response is received back
     *  from the underlying storage system (or timeout occurs).  If
     *  successful, the second parameter contains the values of any
     *  read operations that were requested (or is NULL if none were
     *  requested). If non-empty, ownership transfers to the caller.
     */
    typedef std::tr1::function<void(Result result, ReadSet* rs)> CommitCallback;
    typedef std::tr1::function<void(Result result, int32 count)> CountCallback;

    virtual ~Storage() {};

    /** Service Interface. */
    virtual void start() {}
    virtual void stop() {}


    /** Trigger a lease on a bucket. This is just a signal to the storage layer
     *  that it should try to make sure it can commit transactions for the
     *  bucket. Depending on the implementation and options, this could mean
     *  anything from doing nothing (single process, local operation) to a full
     *  lease or lock (distributed storage).
     */
    virtual void leaseBucket(const Bucket& bucket) = 0;
    /** Release whatever lease or lock was acquired by the corresponding call to
     *  leaseBucket.
     */
    virtual void releaseBucket(const Bucket& bucket) = 0;


    /** Begin a transaction. */
    virtual void beginTransaction(const Bucket& bucket) = 0;

    /** Completes a transaction and requests that it be written to
       Flushes all outstanding events (writes and removes) from pending queue.
       Resets pending queue as well.
    */
    virtual void commitTransaction(const Bucket& bucket, const CommitCallback& cb = 0, const String& timestamp="current") = 0;

   /**
      @param {Key} key the key to erase
      @param {CommitCallback} cb optional commit callback which is
                     invoked if this is a single operation transaction.
      @param {String} timestamp the timestamp of the operation
      @return {bool} true if the erase is queued, false
                     otherwise. Does not indicated success of actual
                     erase operation.

      Queues the item to be removed from the backend.  Does not actually delete
      until the flush operation is called.

      Erasing a non-existant key does not cause an error -- it's just
      treated as a noop. If you care that a key is actually erase, use
      a compare + delete combination.
   */
    virtual bool erase(const Bucket& bucket, const Key& key, const CommitCallback& cb = 0, const String& timestamp="current") = 0;

    /**
       Queues writes to the item named by entryName:itemName.  Writes
       will not be committed until flush command.  Note, if issue this command
       multiple times with the same entryName:itemName, will only process the
       last when calling flush.

       @param {Key} key the key to erase
       @param {String} value What should be written into that item
       @param {CommitCallback} cb optional commit callback which is
                      invoked if this is a single operation transaction.
       @param {String} timestamp the timestamp of the operation

       @return {bool} true if the write is queued, false
                     otherwise. Does not indicated success of actual
                     write operation.
    */
    virtual bool write(const Bucket& bucket, const Key& key, const String& value, const CommitCallback& cb = 0, const String& timestamp="current") = 0;

    /**
       @param {Key} key the key to read
       @param {CommitCallback} cb optional commit callback which is
                      invoked if this is a single operation transaction.
       @param {String} timestamp the timestamp of the operation

       @return {bool} true if the read is queued, false
                     otherwise. Does not indicated success of actual
                     read operation.
     */
    virtual bool read(const Bucket& bucket, const Key& key, const CommitCallback& cb = 0, const String& timestamp="current") = 0;

    /**
       @param {Key} from the start key of range of keys to read
       @param {Key} finish the end key of range of keys to read
       @param {CommitCallback} cb optional commit callback which is
                      invoked if this is a single operation transaction.
       @param {String} timestamp the timestamp of the operation

       @return {bool} true if the read is queued, false
                     otherwise. Does not indicated success of actual
                     read operation.
     */
    virtual bool rangeRead(const Bucket& bucket, const Key& start, const Key& finish, const CommitCallback& cb = 0, const String& timestamp="current") = 0;


    /** Add a comparison operation to the transaction. The given key is read and
        compared to the value. If the comparison is false, it causes the entire
        transaction to fail.

       @param {Key} key the key to compare to
       @param {String} value What should be written into that item
       @param {CommitCallback} cb optional commit callback which is
                      invoked if this is a single operation transaction.
       @param {String} timestamp the timestamp of the operation

       @return {bool} true if the read is queued, false
                     otherwise. Does not indicated success of actual
                     read operation.
     */
    virtual bool compare(const Bucket& bucket, const Key& key, const String& value, const CommitCallback& cb = 0, const String& timestamp="current") = 0;

    /**
       @param {Key} from the start key of range of keys to erase
       @param {Key} finish the end key of range of keys to erase
       @param {CommitCallback} cb optional commit callback which is
                      invoked if this is a single operation transaction.
       @param {String} timestamp the timestamp of the operation

       @return {bool} true if the read is queued, false
                     otherwise. Does not indicated success of actual
                     erase operation.
     */
    virtual bool rangeErase(const Bucket& bucket, const Key& start, const Key& finish, const CommitCallback& cb = 0, const String& timestamp="current") = 0;

    // TODO(ewencp) this interface seems broken -- because it uses a different
    // callback, it doesn't fit in with transactions. This currently *has* to be
    // handled separately from other actions...
    /**
       @param {Key} from the start key of range of keys to count
       @param {Key} finish the end key of range of keys to count
       @param {CommitCallback} cb optional commit callback which is
                      invoked if this is a single operation transaction.
       @param {String} timestamp the timestamp of the operation

       @return {bool} true if operation is successful, false otherwise
     */
    virtual bool count(const Bucket& bucket, const Key& start, const Key& finish, const CountCallback& cb = 0, const String& timestamp="current") = 0;
};


class SIRIKATA_OH_EXPORT StorageFactory
    : public AutoSingleton<StorageFactory>,
      public Factory2<Storage*, ObjectHostContext*, const String&>
{
public:
    static StorageFactory& getSingleton();
    static void destroy();
};

} //end namespace OH
} //end namespace Sirikata

#endif //__SIRIKATA_OH_STORAGE_HPP__
