
#include "SoundSender.hpp"


namespace Sirikata
{
namespace SDL
{

SoundSender::SoundSender(SpaceObjectReference _sporef,HostedObjectPtr hop)
 : sporef(_sporef),
   mDelegateODPService(NULL),
   makerSubmitPort(NULL),
   mHostedObjectPtr(hop),
   dest (SpaceObjectReference::null(),AUDIO_MAKER_PORT)
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

    String msgBody ("test");
    MemoryReference toSend(msgBody);
    //send to space
    makerSubmitPort->send(dest, toSend);

    //lkjs;
    AUDIO_LOG(error,"Fix.  start send of soundsender should actually do something");
    return true;
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
