
#include "SoundListener.hpp"

#define SL_LOG(lvl,msg) SILOG(sound_listener,lvl,msg);

namespace Sirikata
{

SoundListener::SoundListener(const ObjectReference& oref, ODPSST::Stream::Ptr strm)
 : objId (oref),
   stream(strm)
{
    record_stream.initialize(
        stream,
        std::tr1::bind(&SoundListener::handleMessage,this,
            std::tr1::placeholders::_1,livenessToken())
    );
}


void SoundListener::sendSound(MemoryReference toSend)
{
    SL_LOG(error,"Sound listener received a new sound.  Sending");
    record_stream.write(toSend);
}

SoundListener::~SoundListener() 
{
    //nothing to clean up
} 
    


void SoundListener::handleMessage(MemoryReference data, Liveness::Token lt)
{
    SL_LOG(error,"Should not receive messages from sound listener");
}




} //end namespace Sirikata
