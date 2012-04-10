
#include "SoundSender.hpp"
#include "FFmpegMemoryProtocol.hpp"

// FFmpeg doesn't check for C++ in their headers, so we have to wrap all FFmpeg
// includes in extern "C"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "FFmpegGlue.hpp"

namespace Sirikata
{
namespace SDL
{

SoundSender::SoundSender(SpaceObjectReference _sporef,HostedObjectPtr hop)
 : sporef(_sporef),
   mDelegateODPService(NULL),
   makerSubmitPort(NULL),
   mHostedObjectPtr(hop),
   dest (SpaceObjectReference(sporef.space(),ObjectReference::null()),AUDIO_MAKER_PORT)
{
    createMakerSubmitPort();
}



void SoundSender::createMakerSubmitPort()
{
    //listen for any sound maker messages from any object
    makerSubmitPort =
        mHostedObjectPtr->bindODPPort(sporef,AUDIO_MAKER_PORT);
    
    if (makerSubmitPort != NULL)
    {
        //any message we receive on this port will go to handleMsgAcks
        makerSubmitPort->receive(
            std::tr1::bind(&SoundSender::handleMsgAcks, this,
                _1, _2, _3,livenessToken()));
    }
    else
    {
        String errMsg = "Error, could not setup requested ";
        errMsg += "makerSubmitPort.  Undefined behavior to follow";
        AUDIO_LOG(error, errMsg);
    }
}

bool SoundSender::startSend(ClipHandle id ,Transfer::DenseDataPtr data)
{
    if (makerSubmitPort == NULL)
        return false;

    FFmpegMemoryProtocol* dataSource =
        new FFmpegMemoryProtocol("temporaryWebName", data);

    //send all the packets at once.
    uint8 dataBlock[1000];

    AUDIO_LOG(error,"Fix.  startSend of soundsender should actually do something");
    size_t readData = dataSource->read(1000,dataBlock);
    while (readData != (size_t)AVERROR_EOF)
    {
        String msgBody((const char*)dataBlock, readData);
        MemoryReference toSend(msgBody);
        //send to space
        makerSubmitPort->send(dest, toSend);
        readData = dataSource->read(1000,dataBlock);
    }
    return true;

    //FIXME: need to consider what to do with clips that are non-local after
    //they have been sent.
}




void SoundSender::handleMsgAcks (
    const ODP::Endpoint& src, const ODP::Endpoint& dst,
    MemoryReference payload, Liveness::Token lt)
{
    //lkjs;
    AUDIO_LOG(error,"Fix.  Must add msg ack handler to soundsender");
}
    





} //namespace sdl
} //namespace sirikata
