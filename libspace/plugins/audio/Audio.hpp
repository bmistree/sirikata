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
#include <sirikata/core/util/Liveness.hpp>
#include <sirikata/core/network/RecordSSTStream.hpp>
#include <sirikata/core/odp/DelegateService.hpp>


namespace Sirikata {

/** Objects that connect to the space can subscribe to the audio service.

    An object can subscribe as a sound listener.

    Any object can send sound to the space by sending an odp message to the
    audio_maker_port.

    Any object can subscribe for mixed audio by sending a subscribe message to
    audio_listener_subscription_port.  (note: to start with, we skip mixing.)
 */
class Audio : public SpaceModule, public ObjectSessionListener, public Liveness {
public:
    Audio(SpaceContext* ctx);
    ~Audio();
    
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

    //sound listeners
    typedef std::tr1::unordered_map<ObjectReference, SoundListener*,ObjectReference::Hasher> ListenerMap;
    typedef ListenerMap::iterator ListenerMapIter;
    ListenerMap soundListeners;
    Mutex listenerMutex;


    /**
       Stream connection to the object has been set up.  Create an entry in
       sound listeners associated with it, enabling it to receive sounds through
       the passed in stream.
     */
    void handleListenerStream(int err, ODPSST::Stream::Ptr strm,
        Liveness::Token lt);

    void handleMakerStream(int err, ODPSST::Stream::Ptr strm,Liveness::Token lt);

    
    /**
       @param {MemoryReference} data --- Should contain the sound to be played.
       @see startSend of liboh/plugins/sdlaudio/SoundSender.cpp.

       @param {IncomingAudioIndex} inAudIndex --- Each sound sender uses a
       single stream once to send each sound.  This means that after we have
       received a message on a stream, we can destroy all state associated with
       that stream, which is stored in an IncomingAudio struct in iam.
       inAudIndex is the key iam that retrieves these data.
     */
    typedef uint32 IncomingAudioIndex;
    void handleNewAudioMsg(MemoryReference data,
        IncomingAudioIndex inAudIndex, Liveness::Token lt);
    
    Mutex incomingAudioMutex;
    IncomingAudioIndex incomingAudioCounter;
    struct IncomingAudio
    {
        IncomingAudio(
            ODPSST::Stream::Ptr strm,IncomingAudioIndex ider,
            Audio* aud)
         : stream(strm),
           id(ider),
           audio(aud)
        {
            record_stream.initialize(stream,
                std::tr1::bind(&Audio::handleNewAudioMsg, audio,
                    std::tr1::placeholders::_1,id,audio->livenessToken()));
        }
        
        ~IncomingAudio()
        {}
            
        RecordSSTStream<ODPSST::Stream::Ptr> record_stream;
        ODPSST::Stream::Ptr stream;
        IncomingAudioIndex id;
        Audio* audio;
        
    };
    typedef std::tr1::unordered_map<uint32, IncomingAudio*> IncomingAudioMap;
    typedef IncomingAudioMap::iterator IncomingAudioMapIter;
    IncomingAudioMap iam;
    
    
    
};



} // namespace Sirikata

#endif //_SIRIKATA_AUDIO_SPACE_MODULE_HPP_
