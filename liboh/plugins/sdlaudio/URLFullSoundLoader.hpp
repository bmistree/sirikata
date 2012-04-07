#ifndef __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_URL_FULL_SOUND_LOADER_HPP__
#define __SIRIKATA_HOST_PLUGIN_SDL_AUDIO_URL_FULL_SOUND_LOADER_HPP__


#include <sirikata/core/transfer/TransferMediator.hpp>
#include <sirikata/core/transfer/ResourceDownloadTask.hpp>
#include <boost/tr1/unordered_map.hpp>
#include "FullSoundLoader.hpp"
#include "SoundCommon.hpp"
#include <set>
#include <sirikata/core/util/Liveness.hpp>

namespace Sirikata
{
namespace SDL
{

class URLFullSoundLoader;


/**
   All processing done in this should be on the audio strand.  This isn't to say
   that we can't do it somewhere else later.  That's just what the model is now.
 */

class URLFullSoundLoaderManager : public FullSoundLoaderManager,
                                  public Liveness
{
public:
    //all of these are borrowed
    URLFullSoundLoaderManager(
        ClipMap* _clips, Mutex* _mutex,Transfer::TransferPoolPtr tPool,
        Network::IOStrandPtr postCBStrand,FullSoundLoaderCB _cb);
 
    
    ~URLFullSoundLoaderManager();

    /**
       @param {bool} local
         - True if we want the sound to play directly in the browser
         - False if we want the sound to be sent to the space
    */
    virtual bool schedule(
        const Transfer::URI& sound_url, float volume, bool looped,
        ClipHandle id,bool local);
    
private:
    typedef std::pair <URLFullSoundLoader*, std::set<ClipHandle> > SlMapElement;
    typedef std::tr1::unordered_map<String,SlMapElement> SlMap;
    typedef SlMap::iterator SlMapIter;

    
    Mutex slMapMutex;
    SlMap slMap;
    FullSoundLoaderCB cb;
    Transfer::TransferPoolPtr transferPool;
    
    //which strand to tell downloader to post back to (by default, this is just
    //going to be the audio strand.  can get faster/more complicated if choose
    //something else.
    Network::IOStrandPtr postCBStrand;

    
    /**
       Gets called after a download completes.  Allows us to remove files from
       slmap and to issue callback to SDLAudio.
     */
    void downloadFinished(
        FullSoundLoaderStatus status,Transfer::DenseDataPtr data,
        String urlStr, Liveness::Token lt);
};



class URLFullSoundLoader : public FullSoundLoader,
                           public Liveness
{
public:
    URLFullSoundLoader(
        Transfer::URI toGetFrom, Transfer::TransferPoolPtr tpp,
        Network::IOStrandPtr postCBTo);
    
    ~URLFullSoundLoader();

    typedef std::tr1::function< void (FullSoundLoaderStatus, Transfer::DenseDataPtr) > URLFullSoundLoaderCB;
    bool startLoad(URLFullSoundLoaderCB cb);
    
    
private:
    Transfer::URI loadingFrom;
    Transfer::TransferPoolPtr transferPool;
    URLFullSoundLoaderCB cb;
    bool amLoading;

    //Should post callback to this strand
    Network::IOStrandPtr postCBStrand;
    
    void handleFinishedDownload(
        Transfer::ResourceDownloadTaskPtr taskptr,
        Transfer::TransferRequestPtr request,
        Transfer::DenseDataPtr response, const String& url_str,
        Liveness::Token lt);

    Transfer::ResourceDownloadTaskPtr dl;
    
};

}//namespace sdl
}//namespace sirikata

#endif
