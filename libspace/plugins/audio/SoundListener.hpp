#ifndef _SIRIKATA_SOUND_LISTENER_MODULE_HPP_
#define _SIRIKATA_SOUND_LISTENER_MODULE_HPP_

#include <vector>
#include <sirikata/core/util/ObjectReference.hpp>
#include <boost/thread/mutex.hpp>
#include <sirikata/core/odp/DelegateService.hpp>

namespace Sirikata
{


class SoundListener
{
public:
    SoundListener(const ObjectReference& oref, ODP::Port* p);
    ~SoundListener();
    
    //no idea what the internal data should actually look like
//    typedef std::vector<SomeData> SoundSamples;
//    void pushSamples(SoundSamples& newSamples);
    
private:
    typedef boost::mutex Mutex;
    Mutex sampleMutex;
    ObjectReference objId;
    ODP::Port* port;
//    SoundSamples mSamples;
};


}// end namespace Sirikata


#endif // _SIRIKATA_SOUND_LISTENER_MODULE_HPP_
