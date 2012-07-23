
#include "SoundListener.hpp"
#include "FFmpegMemoryProtocol.hpp"
#include "FFmpegAudioStream.hpp"

// FFmpeg doesn't check for C++ in their headers, so we have to wrap all FFmpeg
// includes in extern "C"
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

#include "FFmpegGlue.hpp"


namespace Sirikata
{
namespace SDL
{

SoundListener::SoundListener(SpaceObjectReference _sporef,HostedObjectPtr hop)
 : sporef(_sporef),
   mHostedObjectPtr(hop)
{
    createListenerStream();
}



void SoundListener::createListenerStream()
{
    ODPSST::Stream::Ptr stream = mHostedObjectPtr->getSpaceStream(sporef);
    if (!stream) return;
        
    stream->createChildStream(
        std::tr1::bind(&SoundListener::handleCreatedStream, this,
            std::tr1::placeholders::_1, std::tr1::placeholders::_2,
            livenessToken()),
        NULL, 0,
        AUDIO_LISTENER_PORT, AUDIO_LISTENER_PORT
    );
}


void SoundListener::handleCreatedStream(
    int err, ODPSST::Stream::Ptr strm, Liveness::Token lt)
{
    Liveness::Lock locked (lt);
    if (!locked)
        return;  //object was deleted 
    
    if (err != SST_IMPL_SUCCESS)
    {
        String errMsg = "Failed to create substream for audio listen ";
        errMsg += "messages. This might happen if the audio service ";
        errMsg += "isn't running on the space server. Retrying infinitely.";
        AUDIO_LOG(error, errMsg);

        createListenerStream();
        return;
    }
    
    mStream = strm;
    mRecordStream.initialize(
        mStream,
        std::tr1::bind(&SoundListener::handleMessage, this,
            std::tr1::placeholders::_1,livenessToken())
    );
}


/**
  @param{MemoryReference} data ---
  @param{Liveness::Token} lt --- 
 */
void SoundListener::handleMessage(MemoryReference data, Liveness::Token lt)
{
    Liveness::Lock locked (lt);
    if (!locked)
        return;  //object was deleted 

    
    String errMsg = "The sound listener received a message from ";
    errMsg += "from the space with a sound to play.";
    AUDIO_LOG(error, errMsg);
}





} //namespace sdl
} //namespace sirikata
