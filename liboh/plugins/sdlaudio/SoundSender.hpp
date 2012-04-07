#ifndef __SIRIKATA_SDLAUDIO_SOUND_SENDER_HPP__
#define __SIRIKATA_SDLAUDIO_SOUND_SENDER_HPP__


#include <sirikata/core/transfer/TransferMediator.hpp>
#include <sirikata/core/transfer/ResourceDownloadTask.hpp>
#include <sirikata/core/util/Liveness.hpp>
#include <sirikata/core/odp/DelegateService.hpp>

#include "SoundCommon.hpp"


namespace Sirikata
{
namespace SDL
{

class SoundSender : public Liveness
{
public:
    SoundSender(SpaceObjectReference _sporef);
    ~SoundSender();

    bool startSend(ClipHandle,Transfer::DenseDataPtr);
    
    
private:
    ODP::DelegatePort* createDelegateODPPort(
        ODP::DelegateService* parentService, const SpaceObjectReference& spaceobj,
        ODP::PortID port);

    bool sendODP(
        const ODP::Endpoint& dest_ep, MemoryReference payload, ODP::PortID port);

    void createMakerSubmitPort();

    void handleMsgAcks (
        const ODP::Endpoint& src, const ODP::Endpoint& dst,
        MemoryReference payload, Liveness::Token lt);
    

    
    SpaceObjectReference sporef;
    ODP::DelegateService* mDelegateODPService;

    //any message received on this port 
    ODP::Port* makerSubmitPort;

    ODP::Endpoint dest;
    
};

} //end namespace sdl
} //end namespace sirikata
#endif
