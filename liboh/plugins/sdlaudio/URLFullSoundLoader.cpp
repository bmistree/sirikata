#include "URLFullSoundLoader.hpp"


namespace Sirikata
{
namespace SDL
{

URLFullSoundLoaderManager::URLFullSoundLoaderManager(
    ClipMap* _clips, Mutex* _mutex, Transfer::TransferPoolPtr tPool,
    Network::IOStrandPtr postStrand,FullSoundLoaderCB _cb)
  : FullSoundLoaderManager(_clips,_mutex),
    cb(_cb),
    transferPool(tPool),
    postCBStrand(postStrand)
{}


URLFullSoundLoaderManager::~URLFullSoundLoaderManager()
{
    //nothing to delete because pointers are borrowed.
    Liveness::letDie();
}

bool URLFullSoundLoaderManager::schedule(
    const Transfer::URI& sound_url, float volume, bool looped,ClipHandle id)
{
    AUDIO_LOG(debug,"Requesting download of url " <<sound_url.toString() << " in urlfullsound manager");
    
    Mutex::scoped_lock slMapLock(slMapMutex);
    
    String urlStr = sound_url.toString();
    SlMapIter smi = slMap.find(urlStr);
    bool newLoad = false;
    if (smi == slMap.end())
    {
        newLoad = true;
        //add to map
        URLFullSoundLoader* ufsl =
            new URLFullSoundLoader(sound_url,transferPool,postCBStrand);

        std::set<ClipHandle> clipSet;
        slMap[urlStr] = SlMapElement(ufsl, clipSet);

        smi = slMap.find(urlStr);
    }

    //insert the clip id into slMap.
    smi->second.second.insert(id);    

    
    //actually insert the clip into clips
    {
        //use this to lock mutex;
        Mutex::scoped_lock locker(*mutex); 

        Clip clip;
        clip.stream.reset();
        clip.paused = false;
        clip.volume = volume;
        clip.loop = looped;
        clip.id = id;
        (*clips)[id] = clip;
    }

    if (newLoad)
    {
        smi->second.first->startLoad(
            std::tr1::bind(&URLFullSoundLoaderManager::downloadFinished, this,
                _1, _2, urlStr,livenessToken()));
    }
    return true;
}

void URLFullSoundLoaderManager::downloadFinished(
    FullSoundLoaderStatus status,Transfer::DenseDataPtr data, String urlStr, Liveness::Token lt)
{
    Liveness::Lock locked(lt);
    if (!locked)
        return;

    AUDIO_LOG(debug,"Received download of "<<urlStr<<" in urlfullsoundloadermanager");
    
    std::set<ClipHandle> affected;    
    {
        //potential changes to slMap
        Mutex::scoped_lock slMapLock(slMapMutex);
            
        //remove from slMap, and pass through
        SlMapIter iter = slMap.find(urlStr);
        if (iter == slMap.end())
        {
            cb(status, data,affected);
            return;
        }

        std::swap(affected,iter->second.second);
        delete iter->second.first;
        slMap.erase(iter);
    }

    //notify internal that it can (for instance, start playing the sound).
    cb(status,data,affected);
}



            

URLFullSoundLoader::URLFullSoundLoader(
    Transfer::URI toGetFrom, Transfer::TransferPoolPtr tpp,
    Network::IOStrandPtr postCBTo)
 : loadingFrom(toGetFrom),
   transferPool(tpp),
   cb(NULL),
   amLoading(false),
   postCBStrand(postCBTo)
{}


URLFullSoundLoader::~URLFullSoundLoader()
{
    Liveness::letDie();
}


bool URLFullSoundLoader::startLoad(URLFullSoundLoaderCB _cb)
{
    if (amLoading)
        return false;

    cb = _cb;
    
    amLoading = true;

    dl = Transfer::ResourceDownloadTask::construct(
        loadingFrom, transferPool,
        1.0,
        std::tr1::bind(&URLFullSoundLoader::handleFinishedDownload, this,
            _1, _2, _3,loadingFrom.toString(),livenessToken()));

    dl->start();

    return true;
}


void URLFullSoundLoader::handleFinishedDownload(
    Transfer::ResourceDownloadTaskPtr taskptr,
    Transfer::TransferRequestPtr request,
    Transfer::DenseDataPtr response, const String& url_str,
    Liveness::Token lt)
{
    Liveness::Lock locked(lt);
    if (!locked)
    {
        //urlfullsoundloader was deleted out from under this
        postCBStrand->post(
            std::tr1::bind(cb ,FULL_SOUND_LOAD_FAIL, response));
        return;
    }
    
    // If the download failed, just log it
    if (!response) {
        AUDIO_LOG(error, "Failed to download " << loadingFrom.toString() << " sound file.");
        amLoading = false;

        //note: cannot perform any additional data operations after this call as
        //we may be deleting ourself here.
        postCBStrand->post(
            std::tr1::bind(cb,FULL_SOUND_LOAD_FAIL,response));
        return;
    }

    if (response->size() == 0) {
        AUDIO_LOG(error, "Got zero sized audio file download for " << loadingFrom.toString());
        amLoading = false;
        postCBStrand->post(
            std::tr1::bind(cb,FULL_SOUND_LOAD_FAIL,response));
        return;
    }

    amLoading = false;
    postCBStrand->post(
        std::tr1::bind(cb,FULL_SOUND_LOAD_SUCCEED,response));

}

} //namespace sdl
} //namespace sirikata
