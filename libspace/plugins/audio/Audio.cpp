// Copyright (c) 2011 Sirikata Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "Audio.hpp"


#define AUDIO_LOG(lvl,msg) SILOG(audio,lvl,msg);


// used once: port will listen for any message, from any object.
// message will be a sound datagram.
#define AUDIO_MAKER_PORT 24

//used many times.  any time we receive a listener subscription request,
//we create a separate port for the listener
#define AUDIO_LISTENER_PORT 26



namespace Sirikata {


using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;
using std::tr1::placeholders::_3;


Audio::Audio(SpaceContext* context)
 : SpaceModule(context),
   incomingAudioCounter (0)
{
    AUDIO_LOG(debug,"Constructing space audio module");
}

Audio::~Audio()
{
    Mutex::scoped_lock lock(listenerMutex);

    for (ListenerMapIter iter = soundListeners.begin();
         iter != soundListeners.end(); ++iter)
    {
        delete iter->second;
    }
    soundListeners.clear();
}



void Audio::start() {
    mContext->objectSessionManager()->addListener(this);
}

void Audio::stop() {
    mContext->objectSessionManager()->removeListener(this);
}

void Audio::newSession(ObjectSession* session)
{
    //by default, all objects are listeners.
    ODPSST::Stream::Ptr strmListener = session->getStream();
    if (! strmListener)
        return;
    strmListener->listenSubstream(
        AUDIO_LISTENER_PORT,
        std::tr1::bind(&Audio::handleListenerStream, this, std::tr1::placeholders::_1,
            std::tr1::placeholders::_2, livenessToken())
    );


    //also listen for any objects that make sound
    ODPSST::Stream::Ptr strmMaker = session->getStream();
    if (! strmMaker)
        return;


    AUDIO_LOG(error,"About to create a listening substream");
    strmMaker->listenSubstream(
        AUDIO_MAKER_PORT,
        std::tr1::bind(&Audio::handleMakerStream, this,
            std::tr1::placeholders::_1, std::tr1::placeholders::_2,
            livenessToken())
    );
}

void Audio::handleMakerStream(int err, ODPSST::Stream::Ptr strm,Liveness::Token lt)
{
    if (err != SST_IMPL_SUCCESS)
    {
        AUDIO_LOG(error, "Error listening on sound maker stream on space");
        return;
    }

    Liveness::Lock locked (lt);
    if (!locked)
        return;  //Audio module deleted

    Mutex::scoped_lock lock(listenerMutex);
    IncomingAudioIndex mId = incomingAudioCounter++;
    IncomingAudio* ia = new IncomingAudio(strm,mId,this);
    iam[mId] = ia;
}


void Audio::handleNewAudioMsg(MemoryReference data,
    IncomingAudioIndex inAudIndex, Liveness::Token lt)
{
    Liveness::Lock locked (lt);
    if (!locked)
        return;  //Audio module deleted

    //remove the sound from the incoming audio map
    {
        Mutex::scoped_lock lock(listenerMutex);
        IncomingAudioMapIter iter = iam.find(inAudIndex);
        if (iter == iam.end())
        {
            String errMsg = "Unable to remove element from incoming audio ";
            errMsg += "because could not find associated index.";
            AUDIO_LOG(error, errMsg);
            return;
        }

        delete iter->second;
        iam.erase(iter);
    }

    //now send the received sound (contained in data) to all listeners
    {
        AUDIO_LOG(error,"Received audio and sending it to listeners.");
        Mutex::scoped_lock lock(listenerMutex);
        for (ListenerMapIter iter = soundListeners.begin();
             iter != soundListeners.end(); ++iter)
        {
            iter->second->sendSound(data);
        }
    }
}
    


void Audio::handleListenerStream(
    int err, ODPSST::Stream::Ptr strm, Liveness::Token lt)
{
    Liveness::Lock locked (lt);
    if (!locked)
        return;  //Audio module deleted

    
    //add the object to listeners
    
    ObjectReference id = strm->remoteEndPoint().endPoint.object();
    AUDIO_LOG(error, "Received listener connection for " << id);

    SoundListener* sndList = new SoundListener(id,strm);
    Mutex::scoped_lock lock(listenerMutex);
    soundListeners[id] = sndList;
}

void Audio::sessionClosed(ObjectSession *session)
{
    ODPSST::Stream::Ptr strm = session->getStream();
    if (strm)
    {
        strm->unlistenSubstream(AUDIO_LISTENER_PORT);
        strm->unlistenSubstream(AUDIO_MAKER_PORT);
    }
    
    const ObjectReference& objId = session->id();
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
