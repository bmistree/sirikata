// Copyright (c) 2011 Sirikata Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#ifndef _SIRIKATA_AUDIO_SPACE_MODULE_HPP_
#define _SIRIKATA_AUDIO_SPACE_MODULE_HPP_

#include <sirikata/space/SpaceModule.hpp>
#include <sirikata/space/ObjectSessionManager.hpp>
#include "SoundMaker.hpp"
#include "SoundListener.hpp"
#include <boost/tr1/unordered_map.hpp>


#include <sirikata/core/odp/DelegateService.hpp>


namespace Sirikata {

/** Objects that connect to the space can subscribe to the audio service.

    An object can subscribe as a sound listener.

    Any object can send sound to the space by sending an odp message to the
    audio_maker_port.

    Any object can subscribe for mixed audio by sending a subscribe message to
    audio_listener_subscription_port.  (note: to start with, we skip mixing.)
 */
class Audio : public SpaceModule, public ObjectSessionListener {
public:
    Audio(SpaceContext* ctx);

    virtual void start();
    virtual void stop();

private:
    // ObjectSessionListener Interface
    virtual void newSession(ObjectSession* session);
    virtual void sessionClosed(ObjectSession *session);  //if a session closes
                                                         //without the object
                                                         //having sent an audio
                                                         //unsubscribe first,
                                                         //then we remove it here.

    
    typedef boost::mutex Mutex;

    //sound makers
    typedef std::tr1::unordered_map<ObjectReference, SoundMaker*, ObjectReference::Hasher> MakerMap;
    typedef MakerMap::iterator MakerMapIter;
    typedef MakerMap::const_iterator MakerMapCIter;
    MakerMap soundMakers; //eventually would want a semaphore to control this
    Mutex makerMutex;

    //sound listeners
    typedef std::tr1::unordered_map<ObjectReference, SoundListener*,ObjectReference::Hasher> ListenerMap;
    ListenerMap soundListeners;
    Mutex listenerMutex;

    
    ODP::DelegateService* mDelegateODPService;

    //any message received on this port 
    ODP::Port* makerSubmitPort;
    ODP::Port* listenerSubscriptionPort;


    /**
       Gets called whenever we use mDelegateODPService to create a port.
     */
    ODP::DelegatePort* createDelegateODPPort(
        ODP::DelegateService* parentService, const SpaceObjectReference& spaceobj,
        ODP::PortID port);

    /**
       Called in constructor.  Creates and begins listening on makerSubmit port
       and listenerSubscription port, respectively.
     */
    void createMakerSubmitPort();
    void createListenerSubscriptionPort();

    
    /**
       All odp messages that we send (through mDelegateODPService) flow through
       this function.
     */
    bool sendODP(
        const ODP::Endpoint& dest_ep, MemoryReference payload, ODP::PortID port);


    /**
       When a message is sent to the sound maker port or the listener
       subscription port, these functions handle that message, respectively.

       payload of makerMessage should be parsed with
       Sirikata::Audio::Protocol::SoundMakerMsg

       payload of listenerSubscriptionMessage should be parsed with
       Sirikata::Audio::Protocol::ListenerSubscribeMsg
       or
       Sirikata::Audio::Protocol::ListenerUnsubscribeMsg

       Otherwise, message is discarded
     */
    void handleMakerMessageIn (
        const ODP::Endpoint& src, const ODP::Endpoint& dst, MemoryReference payload);
    void handleListenerSubscriptionMessageIn (
        const ODP::Endpoint& src, const ODP::Endpoint& dst, MemoryReference payload);
    
};



} // namespace Sirikata

#endif //_SIRIKATA_AUDIO_SPACE_MODULE_HPP_
