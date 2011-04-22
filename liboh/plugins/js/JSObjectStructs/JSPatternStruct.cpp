#include "JSPatternStruct.hpp"
#include <v8.h>

namespace Sirikata {
namespace JS {


using namespace v8;
Pattern::Pattern(const std::string& _name,v8::Handle<v8::Value> _value,v8::Handle<v8::Value> _proto)
 :mName(_name), mValue(v8::Persistent<v8::Value>::New(_value)), mPrototype(v8::Persistent<v8::Value>::New(_proto))
{
}

Pattern::Pattern(v8::Handle<v8::Object>




bool Pattern::matches(v8::Handle<v8::Object> obj) const
{
    if (mName == "")
        return true;
    
    if (!obj->Has(v8::String::New(mName.c_str())))
    {
        return false;
    }
    
    if (hasValue())
    {
        Handle<Value> field = obj->Get(v8::String::New(mName.c_str()));

        if (!field->Equals(mValue))
            return false;

    }

    if (hasPrototype()) {
        // FIXME check prototype
    }

    return true;
}


//prints a pattern
void Pattern::printPattern() const
{
    //name
    std::cout<<"Name: "<<mName.c_str()<<"\n";

    if (hasValue())
    {
        //std::cout<<" Having hard time with value\n";
        v8::String::Utf8Value stringValue(mValue);
        const char* strval = ToCString(stringValue);
        std::string stringVal (strval);
        std::cout<<"  Value: "<<stringVal<<"\n";
    }
    else
        std::cout<<"  Value: ANY\n";

    //FIXME: Prototype still needs to be printed.
}





} //namespace js
} //namespace sirikata
