
#include "SoundSender.hpp"


namespace Sirikata
{
namespace SDL
{

SoundSender::SoundSender(SpaceObjectReference _sporef)
 : sporef(_sporef),
   mDelegateODPService(NULL),
   makerSubmitPort(NULL),
   dest (SpaceObjectReference::null(),AUDIO_MAKER_PORT)
{
    mDelegateODPService = new ODP::DelegateService(
        std::tr1::bind(
            &SoundSender::createDelegateODPPort, this,
            _1, _2, _3
        ));
}



void SoundSender::createMakerSubmitPort()
{
    //listen for any sound maker messages from any object
    makerSubmitPort =
        mDelegateODPService->bindODPPort(sporef,AUDIO_MAKER_PORT);
    
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

bool SoundSender::sendODP(
    const ODP::Endpoint& dest_ep, MemoryReference payload, ODP::PortID port)
{
    //lkjs;
    AUDIO_LOG(error,"Fix.  Must add send functionality to soundsender");
    return true;
}



void SoundSender::handleMsgAcks (
    const ODP::Endpoint& src, const ODP::Endpoint& dst,
    MemoryReference payload, Liveness::Token lt)
{
    //lkjs;
    AUDIO_LOG(error,"Fix.  Must add msg ack handler to soundsender");
}
    



ODP::DelegatePort* SoundSender::createDelegateODPPort(
    ODP::DelegateService* parentService, const SpaceObjectReference& spaceobj,
    ODP::PortID port)
{
    ODP::Endpoint port_ep(spaceobj,port);
    
    return new ODP::DelegatePort(
        mDelegateODPService,
        port_ep,
        std::tr1::bind(
            &SoundSender::sendODP, this,
            _1, _2, port));
}




} //namespace sdl
} //namespace sirikata
