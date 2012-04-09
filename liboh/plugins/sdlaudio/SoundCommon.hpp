#ifndef __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_SOUND_COMMON_HPP__
#define __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_SOUND_COMMON_HPP__

#include <map>
#include <boost/thread/mutex.hpp>
#include <functional>

#define AUDIO_LOG(lvl, msg) SILOG(sdl-audio, lvl, msg)

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

#include <sirikata/oh/HostedObject.hpp>


namespace Sirikata
{
namespace SDL
{

class FFmpegAudioStream;
typedef std::tr1::shared_ptr<FFmpegAudioStream> FFmpegAudioStreamPtr;


typedef boost::mutex Mutex;
typedef boost::unique_lock<Mutex> Lock;

typedef uint32 ClipHandle;

struct Clip {
    FFmpegAudioStreamPtr stream;
    bool paused;
    float32 volume;
    bool loop;
    ClipHandle id;
    bool local;
};
typedef std::map<ClipHandle, Clip> ClipMap;


enum FullSoundLoaderStatus
{
    FULL_SOUND_LOAD_FAIL,
    FULL_SOUND_LOAD_SUCCEED
};
typedef std::tr1::function< void (FullSoundLoaderStatus,Transfer::DenseDataPtr,std::set<ClipHandle>) > FullSoundLoaderCB;


}//close namespace sdl
} //close namespace sirikata

#endif //__SIRIKATA_HOST_PLUGIN_SDL_AUDIO_SOUND_COMMON_HPP__
