#ifndef __SIRIKATA_SDLAUDIO_SOUND_LISTENER_HPP__
#define __SIRIKATA_SDLAUDIO_SOUND_LISTENER_HPP__


#include <sirikata/core/transfer/TransferMediator.hpp>
#include <sirikata/core/transfer/ResourceDownloadTask.hpp>
#include <sirikata/core/util/Liveness.hpp>
#include <sirikata/core/odp/DelegateService.hpp>
#include <sirikata/oh/Platform.hpp>
#include <sirikata/core/network/RecordSSTStream.hpp>

#include "SoundCommon.hpp"


namespace Sirikata
{
namespace SDL
{


class SoundListener : public Liveness
{
public:
    SoundListener(SpaceObjectReference _sporef,HostedObjectPtr hop);
    ~SoundListener();

    
private:

    /**
       Sound sender has a stream, which it uses to forward sounds to the space
       server.
     */
    void createListenerStream();

    /**
       Sets up mStream and mRecordStream
     */
    void handleCreatedStream(int err, ODPSST::Stream::Ptr strm,Liveness::Token);
    

    SpaceObjectReference sporef;

    // Communication with the space
    ODPSST::Stream::Ptr mStream;
    //record stream provides callbacks when full message is sent, not when some
    //number of bytes are sent.
    RecordSSTStream<ODPSST::Stream::Ptr> mRecordStream;
    HostedObjectPtr mHostedObjectPtr;

    void handleMessage(MemoryReference data, Liveness::Token lt);
    
};

} //end namespace sdl
} //end namespace sirikata
#endif
