#ifndef __SIRIKATA_SDLAUDIO_SOUND_SENDER_HPP__
#define __SIRIKATA_SDLAUDIO_SOUND_SENDER_HPP__


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


class SoundSender : public Liveness
{
public:
    SoundSender(SpaceObjectReference _sporef,HostedObjectPtr hop);
    ~SoundSender();


    /**
       Shoves data contained in DenseDataPtr into a reliable stream and sends it
       to the space.  The space then forwards it verbatim to all subscribed
       sound listeners.
     */
    bool startSend(ClipHandle,Transfer::DenseDataPtr);
    
    
private:

    /**
       Sound sender has a stream, which it uses to forward sounds to the space
       server.
     */
    void createMakerSubmitStream();

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
    
    //any message received on this port 
    ODP::Port* makerSubmitPort;

    
    ODP::Endpoint dest;
    
    
};

} //end namespace sdl
} //end namespace sirikata
#endif
