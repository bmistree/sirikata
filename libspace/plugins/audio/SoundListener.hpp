#ifndef _SIRIKATA_SOUND_LISTENER_MODULE_HPP_
#define _SIRIKATA_SOUND_LISTENER_MODULE_HPP_

#include <vector>
#include <sirikata/core/util/ObjectReference.hpp>
#include <boost/thread/mutex.hpp>
#include <sirikata/core/odp/DelegateService.hpp>
#include <sirikata/core/util/Liveness.hpp>
#include <sirikata/core/network/RecordSSTStream.hpp>
#include <sirikata/space/ObjectSessionManager.hpp>

namespace Sirikata
{


class SoundListener : public Liveness
{
public:
    SoundListener(const ObjectReference& oref, ODPSST::Stream::Ptr strm);
    ~SoundListener();
    void handleMessage(MemoryReference data, Liveness::Token lt);
    void sendSound(MemoryReference toSend);
    
    
private:
    ObjectReference objId;
    RecordSSTStream<ODPSST::Stream::Ptr> record_stream;
    ODPSST::Stream::Ptr stream;
    
};


}// end namespace Sirikata


#endif // _SIRIKATA_SOUND_LISTENER_MODULE_HPP_
