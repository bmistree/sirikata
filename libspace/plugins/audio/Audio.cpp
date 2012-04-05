// Copyright (c) 2011 Sirikata Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "Audio.hpp"


#define AUDIO_LOG(lvl,msg) SILOG(audio,lvl,msg);


// used once: port will listen for any message, from any object.
// message will be a sound datagram.
#define AUDIO_MAKER_PORT 24

//used once: port will listen for any message from any object that wants
//to either subscribe to listening sounds or unsubscribe from listening
//to sounds.
#define AUDIO_LISTENER_SUBSCRIPTION_PORT 25

//used many times.  any time we receive a listener subscription request,
//we create a separate port for the listener
#define AUDIO_LISTENER_PORT 26



namespace Sirikata {


using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;
using std::tr1::placeholders::_3;


Audio::Audio(SpaceContext* context)
 : SpaceModule(context)
{
    //whenever created a new port, put 
    mDelegateODPService = new ODP::DelegateService(
        std::tr1::bind(
            &Audio::createDelegateODPPort, this,
            _1, _2, _3
        )
    );

    createMakerSubmitPort();
    createListenerSubscriptionPort();
}

void Audio::createMakerSubmitPort()
{
    //listen for any sound maker messages from any object
    makerSubmitPort =
        mDelegateODPService->bindODPPort(
            SpaceObjectReference::null(),AUDIO_MAKER_PORT);

    if (makerSubmitPort != NULL)
    {
        //any message we receive on this port will go to handleMakerMessageIn
        makerSubmitPort->receive(
            std::tr1::bind(&Audio::handleMakerMessageIn, this, _1, _2, _3));
    }
    else
    {
        String errMsg = "Error, could not setup requested ";
        errMsg += "makerSubmitPort.  Undefined behavior to follow";
        AUDIO_LOG(error, errMsg);
    }
}

void Audio::createListenerSubscriptionPort()
{
    //listen for any subscription messages from any object
    listenerSubscriptionPort =
        mDelegateODPService->bindODPPort(
            SpaceObjectReference::null(),AUDIO_LISTENER_SUBSCRIPTION_PORT);

    if (listenerSubscriptionPort != NULL)
    {
        //any message that we receive on this port will go to
        //handleListenerSubscriptionMessageIn
        listenerSubscriptionPort->receive(
            std::tr1::bind(&Audio::handleListenerSubscriptionMessageIn, this, _1, _2, _3));
    }
    else
    {
        String errMsg = "Error, could not setup requested ";
        errMsg += "listenerSubscriptionPort.  Undefined behavior to follow";
        AUDIO_LOG(error, errMsg);
    }
}

    

bool Audio::sendODP(
    const ODP::Endpoint& dest_ep, MemoryReference payload, ODP::PortID port)
{
    AUDIO_LOG(error, "Error.  have not set up sending odp messages in audio space plugin.");
    assert(false);
    return true;
}



ODP::DelegatePort* Audio::createDelegateODPPort(
    ODP::DelegateService* parentService, const SpaceObjectReference& spaceobj,
    ODP::PortID port)
{
    ODP::Endpoint port_ep(spaceobj,port);

    AUDIO_LOG(debug, "Creating for port "<<port);
    
    return new ODP::DelegatePort(
        mDelegateODPService,
        port_ep,
        std::tr1::bind(
            &Audio::sendODP, this,
            _1, _2, port));
}



void Audio::start() {
    mContext->objectSessionManager()->addListener(this);
}

void Audio::stop() {
    mContext->objectSessionManager()->removeListener(this);
}

void Audio::newSession(ObjectSession* session)
{
    //ignore.  not interested in objects unless they specifically subscribe to
    //listen or send a noise maker message.
}

void Audio::handleMakerMessageIn (
    const ODP::Endpoint& src, const ODP::Endpoint& dst, MemoryReference payload)
{
    AUDIO_LOG(error,"Received a message from a sound maker");
}

void Audio::handleListenerSubscriptionMessageIn (
    const ODP::Endpoint& src, const ODP::Endpoint& dst, MemoryReference payload)
{
    AUDIO_LOG(error,"Received a message for a listen subscription");
}


void Audio::sessionClosed(ObjectSession *session)
{
    const ObjectReference& objId = session->id();

    {
        Mutex::scoped_lock lock(makerMutex);
        if (soundMakers.find(objId) != soundMakers.end())
        {
            SoundMaker* sm = soundMakers[objId];
            delete sm;
            soundMakers.erase(objId);
        }
    }

    {
        Mutex::scoped_lock lock(listenerMutex);
        if (soundListeners.find(objId) != soundListeners.end())
        {
            SoundListener* sl = soundListeners[objId];
            delete sl;
            soundListeners.erase(objId);
        }
    }
}


} // namespace Sirikata
