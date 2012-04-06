#ifndef __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_SOUND_COMMON_HPP__
#define __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_SOUND_COMMON_HPP__

#include <map>
#include <boost/thread/mutex.hpp>
#define AUDIO_LOG(lvl, msg) SILOG(sdl-audio, lvl, msg)

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
