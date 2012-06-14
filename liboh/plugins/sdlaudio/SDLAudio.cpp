// Copyright (c) 2011 Sirikata Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include "SDLAudio.hpp"

#include <sirikata/core/transfer/AggregatedTransferPool.hpp>
#include <sirikata/core/network/IOStrandImpl.hpp>

#include <sirikata/sdl/SDL.hpp>
#include "SDL_audio.h"

#include "FFmpegGlue.hpp"
#include "FFmpegMemoryProtocol.hpp"
#include "FFmpegStream.hpp"
#include "FFmpegAudioStream.hpp"


namespace Sirikata {
namespace SDL {

namespace {

AudioSimulation* ToSimulation(void* data) {
    return reinterpret_cast<AudioSimulation*>(data);
}

extern void mixaudio(void* _sim, Uint8* raw_stream, int raw_len) {
    AUDIO_LOG(insane, "Mixing audio samples");

    AudioSimulation* sim = ToSimulation(_sim);
    sim->mix(raw_stream, raw_len);
}

}

AudioSimulation::AudioSimulation(
    Context* ctx,Network::IOStrandPtr ptr,const SpaceObjectReference& sporef,
    HostedObjectPtr hop)
 : audioStrand(ptr),
   mContext(ctx),
   mInitializedAudio(false),
   mOpenedAudio(false),
   mSporef(sporef),
   mClipHandleSource(0),
   mPlaying(false),
   mURLFullSoundLoaderManager(NULL),
   mSoundSender(NULL),
   mHostedObjectPtr(hop)
{
}


AudioSimulation::~AudioSimulation()
{
    /**
       FIXME: delete mURLFullSoundLoaderManger if not null;
     */
    delete mURLFullSoundLoaderManager;
    
    Liveness::letDie();
}

void AudioSimulation::start()
{
    audioStrand->post(
        std::tr1::bind(&AudioSimulation::iStart, this,livenessToken()),
        "AudioSimulation::iStart"
    );
}

void AudioSimulation::iStart(Liveness::Token lt)
{
    if (!lt) return;
    Liveness::Lock locked(lt);
    if (!locked)
    {
        AUDIO_LOG(warn,"Did not finish internal start "<<\
            "of AudioSimulation: expired audiosim.");
        return;
    }

    AUDIO_LOG(debug, "Starting SDLAudio");

    if (SDL::InitializeSubsystem(SDL::Subsystem::Audio) != 0)
        return;

    mInitializedAudio = true;

    SDL_AudioSpec fmt;

    /* Set 16-bit stereo audio at 44Khz */
    fmt.freq = 44100;
    fmt.format = AUDIO_S16;
    fmt.channels = 2;
    fmt.samples = 2048;
    fmt.callback = mixaudio;
    fmt.userdata = this;

    /* Open the audio device and start playing sound! */
    if ( SDL_OpenAudio(&fmt, NULL) < 0 ) {
        AUDIO_LOG(error, "Unable to open audio: " << SDL_GetError());
        return;
    }

    mOpenedAudio = true;

    FFmpegGlue::getSingleton().ref();


    //to play clips from urls
    mTransferPool = Transfer::TransferMediator::getSingleton().registerClient<Transfer::AggregatedTransferPool>("SDLAudio");
    
    mURLFullSoundLoaderManager =
        new URLFullSoundLoaderManager(
            &mClips,&mMutex,mTransferPool,audioStrand,
            audioStrand->wrap(
                std::tr1::bind(&AudioSimulation::downloadFinished,this,
                    _1, _2, _3,livenessToken())));

    mSoundSender = new SoundSender(mSporef,mHostedObjectPtr);
    //lkjs;
}



bool AudioSimulation::ready() const {
    return (mInitializedAudio && mOpenedAudio && mTransferPool);
}

void AudioSimulation::stop()
{
    audioStrand->post(
        std::tr1::bind(&AudioSimulation::iStop,this,livenessToken()),
        "AudioSimulation::iStop"
    );
}

void AudioSimulation::iStop(Liveness::Token lt)
{
    if (!lt) return;
    Liveness::Lock locked(lt);
    if (!locked)
    {
        AUDIO_LOG(warn,"Did not finish internal stop "<<\
            "of AudioSimulation: expired audiosim.");
        return;
    }

    AUDIO_LOG(detailed, "Stopping SDLAudio");

    mTransferPool.reset();
    mDownloads.clear();

    if (!mInitializedAudio)
        return;

    if (mOpenedAudio) {
        SDL_PauseAudio(1);
        SDL_CloseAudio();

        FFmpegGlue::getSingleton().unref();
    }

    SDL::QuitSubsystem(SDL::Subsystem::Audio);
    mInitializedAudio = false;
}



boost::any AudioSimulation::invoke(std::vector<boost::any>& params)
{
    // Decode the command. First argument is the "function name"
    if (params.empty() || !Invokable::anyIsString(params[0]))
        return boost::any();

    std::string name = Invokable::anyAsString(params[0]);
    AUDIO_LOG(detailed, "Invoking the function " << name);

    if (name == "play") 
        return startDownloadAndPlayFromURL(params);
    if (name == "playSpaceSound")
        return startDownloadAndSendToSpaceFromURL(params);
    else if (name == "stop") {
        if (params.size() < 2 || !Invokable::anyIsNumeric(params[1]))
            return boost::any();
        ClipHandle id = Invokable::anyAsNumeric(params[1]);

        AUDIO_LOG(detailed, "Stop request for ID " << id);

        Mutex::scoped_lock lck(mMutex);

        // Clear out from playing streams
        ClipMap::iterator clip_it = mClips.find(id);
        if (clip_it != mClips.end()) {
            AUDIO_LOG(insane, "Stopping actively playing clip ID " << id);
            mClips.erase(clip_it);
        }

        // Clear out from downloads if it's still downloading
        // Not particularly efficient, but hopefully this isn't a very big map
        for(DownloadTaskMap::iterator down_it = mDownloads.begin(); down_it != mDownloads.end(); down_it++) {
            if (down_it->second.waiting.find(id) != down_it->second.waiting.end()) {
                AUDIO_LOG(insane, "Stopping downloading clip ID " << id);
                down_it->second.waiting.erase(id);
                break;
            }
        }
    }
    else if (name == "volume") {
        if (params.size() < 2 || !Invokable::anyIsNumeric(params[1]))
            return boost::any();
        ClipHandle id = Invokable::anyAsNumeric(params[1]);

        if (params.size() < 3 || !Invokable::anyIsNumeric(params[2]))
            return boost::any();
        float32 volume = (float32)Invokable::anyAsNumeric(params[2]);

        Mutex::scoped_lock(mMutex);
        if (mClips.find(id) == mClips.end()) return Invokable::asAny(false);
        mClips[id].volume = volume;

        return Invokable::asAny(true);
    }
    else if (name == "pause" || name == "resume") {
        if (params.size() < 2 || !Invokable::anyIsNumeric(params[1]))
            return boost::any();
        ClipHandle id = Invokable::anyAsNumeric(params[1]);

        bool paused = (name == "pause");
        AUDIO_LOG(detailed, name << " request for ID " << id);

        Mutex::scoped_lock(mMutex);

        ClipMap::iterator clip_it = mClips.find(id);
        if (clip_it != mClips.end()) {
            clip_it->second.paused = paused;
            return Invokable::asAny(true);
        }
        else {
            return Invokable::asAny(false);
        }
    }
    else if (name == "loop") {
        if (params.size() < 2 || !Invokable::anyIsNumeric(params[1]))
            return boost::any();
        ClipHandle id = Invokable::anyAsNumeric(params[1]);

        if (params.size() < 3 || !Invokable::anyIsBoolean(params[2]))
            return boost::any();
        bool looped = Invokable::anyAsBoolean(params[2]);

        Mutex::scoped_lock(mMutex);
        if (mClips.find(id) == mClips.end()) return Invokable::asAny(false);
        mClips[id].loop = looped;

        return Invokable::asAny(true);
    }
    else if (name == "sendSound")
    {
        AUDIO_LOG(error,"Error, have not permitted send sound functionality yet.");
        //return sendSound(params);
    }
    else {
        AUDIO_LOG(warn, "Function " << name << " was invoked but this function was not found.");
    }

    return boost::any();
}



/**
   gets us to send a local sound file, bit-by-bit to the space server,
   which will mix it and send sound to listeners.

   @param params

   params[0] ---> toString should give "sendSound"

   params[1] ---> toString (for now) should give local file to send in pieces to
   the space.

   Returns true if works, boost::any() otherwise.
 */
boost::any sendSound(std::vector<boost::any>& params)
{

    if (params.size() != 2)
        return boost::any();

    String name = Invokable::anyAsString(params[0]);
    if (name != "sendSound")
        return boost::any();

    String filePath = Invokable::anyAsString(params[1]);
    AUDIO_LOG(debug,"Sending sound from local file "<< filePath<<" to space");
    //not finished yet
    assert(false);

    return Invokable::asAny(true);
}



void AudioSimulation::downloadFinished(
    FullSoundLoaderStatus status,Transfer::DenseDataPtr response,
    std::set<ClipHandle> waitingClips,Liveness::Token lt)
{
    if (status == FULL_SOUND_LOAD_FAIL)
        return;
    
    Liveness::Lock locked(lt);
    if (!locked)
    {
        AUDIO_LOG(detailed, "Not playing download because audio simulation canceled");
        return;
    }
    
    Mutex::scoped_lock lock(mMutex);

    //start playing the selected clips.
    for(std::set<ClipHandle>::iterator id_it = waitingClips.begin();
        id_it != waitingClips.end(); id_it++)
    {
        // Might have been removed already
        if (mClips.find(*id_it) == mClips.end()) continue;

        if (!mClips[*id_it].local)
        {
            //means that we are supposed to transmit this sound to the space
            //server for mixing and delivery to others.
            mSoundSender->startSend(*id_it,response);
            continue;
        }
        FFmpegMemoryProtocol* dataSource =
            new FFmpegMemoryProtocol("temporaryWebName", response);
        
        FFmpegStreamPtr stream(FFmpegStream::construct<FFmpegStream>(
                static_cast<FFmpegURLProtocol*>(dataSource)));
        
        if (stream->numAudioStreams() == 0) {
            AUDIO_LOG(error, "Found zero audio streams, ignoring");
            return;
        }
        if (stream->numAudioStreams() > 1)
            AUDIO_LOG(detailed, "Found more than one audio stream, only playing first");
        
        FFmpegAudioStreamPtr audio_stream = stream->getAudioStream(0, 2);
        mClips[*id_it].stream = audio_stream;
    }

    // Enable playback if we didn't have any active streams before
    if (!mPlaying) {
        SDL_PauseAudio(0);
        mPlaying = true;
    }
}



boost::any AudioSimulation::startDownloadAndSendToSpaceFromURL(
    std::vector<boost::any>& params)
{
    // Ignore if we didn't initialize properly
    if (!ready())
        return boost::any();

    String name = Invokable::anyAsString(params[0]);
    if (name != "playSpaceSound")
        return boost::any();

    // URL
    if (params.size() < 2 || !Invokable::anyIsString(params[1]))
        return boost::any();
    String sound_url_str = Invokable::anyAsString(params[1]);
    Transfer::URI sound_url(sound_url_str);
    if (sound_url.empty()) return boost::any();
    // Volume
    float32 volume = 1.f;
    if (params.size() >= 3 && Invokable::anyIsNumeric(params[2]))
        volume = (float32)Invokable::anyAsNumeric(params[2]);

    // Do not permit looping sounds in space
    bool looped = false;

    //actually initiate the download from the url.
    ClipHandle id = incrementClipId();
    mURLFullSoundLoaderManager->schedule(sound_url,volume,looped,id,false);

    return boost::any(id);
}



boost::any AudioSimulation::startDownloadAndPlayFromURL(
    std::vector<boost::any>& params)
{
    // Ignore if we didn't initialize properly
    if (!ready())
        return boost::any();

    String name = Invokable::anyAsString(params[0]);
    if (name != "play")
        return boost::any();

    // URL
    if (params.size() < 2 || !Invokable::anyIsString(params[1]))
        return boost::any();
    String sound_url_str = Invokable::anyAsString(params[1]);
    Transfer::URI sound_url(sound_url_str);
    if (sound_url.empty()) return boost::any();
    // Volume
    float32 volume = 1.f;
    if (params.size() >= 3 && Invokable::anyIsNumeric(params[2]))
        volume = (float32)Invokable::anyAsNumeric(params[2]);
    // Looping
    bool looped = false;
    if (params.size() >= 4 && Invokable::anyIsBoolean(params[3]))
        looped = Invokable::anyAsBoolean(params[3]);

    //actually initiate the download from the url.
    ClipHandle id = incrementClipId();
    mURLFullSoundLoaderManager->schedule(sound_url,volume,looped,id,true);

    return boost::any(id);
}


ClipHandle AudioSimulation::incrementClipId()
{
    ClipHandle returner = mClipHandleSource++;
    if (returner == CLIP_HANDLE_NULL)
        return incrementClipId();

    return returner;
}



void AudioSimulation::mix(uint8* raw_stream, int32 raw_len) {
    int16* stream = (int16*)raw_stream;
    // Length in individual samples
    int32 stream_len = raw_len / sizeof(int16);
    // Length in samples for all channels
#define MAX_CHANNELS 6
    int32 nchannels = 2; // Assuming stereo, see SDL audio setup
    int32 samples_len = stream_len / nchannels;

    Mutex::scoped_lock(mMutex);


    for(int i = 0; i < samples_len; i++) {
        int32 mixed[MAX_CHANNELS];
        for(int c = 0; c < nchannels; c++)
            mixed[c] = 0;

        for(ClipMap::iterator st_it = mClips.begin(); st_it != mClips.end(); st_it++) {
            // We might still be downloading it or it might be paused
            if (!st_it->second.stream ||
                st_it->second.paused)
                continue;

            int16 samples[MAX_CHANNELS];
            st_it->second.stream->samples(samples, st_it->second.loop);

            for(int c = 0; c < nchannels; c++)
                mixed[c] += samples[c] * st_it->second.volume;
        }

        for(int c = 0; c < nchannels; c++)
            stream[i*nchannels + c] = (int16)std::min(std::max(mixed[c], -32768), 32767);
    }

    // Clean out local streams that have finished
    for(ClipMap::iterator st_it = mClips.begin(); st_it != mClips.end(); )
    {
        ClipMap::iterator del_it = st_it;
        if (del_it->second.local && del_it->second.stream->finished())
            mClips.erase(del_it);
        
        st_it++;
    }

    
    // Disable playback if we've run out of sounds
    if (mClips.empty()) {
        SDL_PauseAudio(1);
        mPlaying = false;
    }
}

} //namespace SDL
} //namespace Sirikata
