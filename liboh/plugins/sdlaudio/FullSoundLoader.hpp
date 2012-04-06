#ifndef __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_FULL_SOUND_LOADER_HPP__
#define __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_FULL_SOUND_LOADER_HPP__

#include "SoundCommon.hpp"


namespace Sirikata
{
namespace SDL
{

class FullSoundLoaderManager
{
public:
    FullSoundLoaderManager(ClipMap* _clips,Mutex* _mutex)
     :clips(_clips),
      mutex(_mutex)
    {}

    virtual ~FullSoundLoaderManager()
    {}
    
    virtual bool schedule(
        const Transfer::URI& sound_url, float volume, bool looped,ClipHandle id) = 0;
    
protected:
    ClipMap* clips;
    Mutex* mutex;
};


/**
   Abstract class.  Issues callback after file is finished.
 */

class FullSoundLoader
{
public:
    // bool startLoad(FullSoundLoaderCB) = 0;
    
};

} //namespace sdl
} //namespace sirikata

#endif
