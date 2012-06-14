
#include "SoundSender.hpp"
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

SoundSender::SoundSender(SpaceObjectReference _sporef,HostedObjectPtr hop)
 : sporef(_sporef),
   mHostedObjectPtr(hop),
   dest (SpaceObjectReference(sporef.space(),ObjectReference::null()),AUDIO_MAKER_PORT)
{
    createMakerSubmitStream();
}



void SoundSender::createMakerSubmitStream()
{
    ODPSST::Stream::Ptr stream = mHostedObjectPtr->getSpaceStream(sporef);
    if (!stream) return;
    
    
    stream->createChildStream(
        std::tr1::bind(&SoundSender::handleCreatedStream, this,
            std::tr1::placeholders::_1, std::tr1::placeholders::_2,
            livenessToken()),
        NULL, 0,
        AUDIO_MAKER_PORT, AUDIO_MAKER_PORT
    );
}


void SoundSender::handleCreatedStream(int err, ODPSST::Stream::Ptr strm, Liveness::Token lt)
{
    Liveness::Lock locked (lt);
    if (!locked)
        return;  //object was deleted 
    
    if (err != SST_IMPL_SUCCESS)
    {
        String errMsg = "Failed to create substream for audio send ";
        errMsg += "messages. This might happen if the audio service ";
        errMsg += "isn't running on the space server.";
        AUDIO_LOG(error, errMsg);

        createMakerSubmitStream();
        
        return;
    }
    
    mStream = strm;
    mRecordStream.initialize(
        mStream,
        std::tr1::bind(&SoundSender::handleMessage, this,
            std::tr1::placeholders::_1,livenessToken())
    );
}


void SoundSender::handleMessage(MemoryReference data, Liveness::Token lt)
{
    Liveness::Lock locked (lt);
    if (!locked)
        return;  //object was deleted 
    
    
    String errMsg = "The audio sender does not expect to ";
    errMsg += "get messages back from the space.  Something ";
    errMsg += "is going wrong.";
    AUDIO_LOG(error, errMsg);
}



bool SoundSender::startSend(ClipHandle id, Transfer::DenseDataPtr data)
{
    String logMsg = "Got into startSend for a clip.";
    AUDIO_LOG(error,logMsg);
    
    if (! mStream)
    {
        String errMsg = "Could not send clip because improperly initialized ";
        errMsg += "audio sst stream.";
        AUDIO_LOG(error,errMsg);
        return false;
    }
    
    AUDIO_LOG(info,"sending data");
    MemoryReference toSend = MemoryReference( (const void*) data->begin(), data->length());
    mRecordStream.write(toSend);
    return true;
}



} //namespace sdl
} //namespace sirikata
