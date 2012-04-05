
#include "SoundListener.hpp"

#define SL_LOG(lvl,msg) SILOG(sound_listener,lvl,msg);

namespace Sirikata
{


SoundListener::SoundListener(const ObjectReference& oref, ODP::Port* p)
 : objId(oref),
   port(p)
{
}


SoundListener::~SoundListener()
{
    delete port;
}


// void SoundMaker::pushSamples(SoundSamples& newSamples)
// {
//     std::cout<<"\npush samples is not finished.  needs to be.\n";
//     assert(false);
// }


} //end namespace Sirikata
