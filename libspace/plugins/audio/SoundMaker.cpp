
#include "SoundMaker.hpp"


#define SM_LOG(lvl,msg) SILOG(sound_maker,lvl,msg);

namespace Sirikata
{

SoundMaker::SoundMaker(const ObjectReference& oref)
 : objId(oref)
{
}


SoundMaker::~SoundMaker()
{
}


// void SoundMaker::getSamples(SoundSamples& toGet)
// {
//     Mutex::scoped_lock lock(sampleMutex);
//     toGet.swap(mSamples);
// }


} //end namespace Sirikata
