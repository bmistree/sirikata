#ifndef _SIRIKATA_SOUND_MAKER_MODULE_HPP_
#define _SIRIKATA_SOUND_MAKER_MODULE_HPP_

#include <vector>

#include <boost/thread/mutex.hpp>
#include <sirikata/core/util/ObjectReference.hpp>

namespace Sirikata
{


class SoundMaker
{
public:
    SoundMaker(const ObjectReference& oref);
    ~SoundMaker();
    
    //no idea what this should actually look like
    // typedef std::vector<SomeData> SoundSamples;
    // void getSamples(SoundSamples& toGet);
    
private:
    typedef boost::mutex Mutex;
    Mutex sampleMutex;
    ObjectReference objId;

    // SoundSamples mSamples;
};


}// end namespace Sirikata

#endif //_SIRIKATA_SOUND_MAKER_MODULE_HPP_
