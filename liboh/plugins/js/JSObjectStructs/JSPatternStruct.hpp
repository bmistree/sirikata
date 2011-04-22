#ifndef __SIRIKATA_JS_PATTERN_HPP__
#define __SIRIKATA_JS_PATTERN_HPP__

#include "../JSUtil.hpp"
#include <v8.h>
#include <iostream>
#include <iomanip>

namespace Sirikata {
namespace JS {



/** Patterns are single rules for matching an object. A field name
 *  must be specified, and either or both of a prototype and value may
 *  be specified.
 */
class Pattern {
public:
    Pattern(const std::string& _name,v8::Handle<v8::Value> _value = v8::Handle<v8::Value>(),v8::Handle<v8::Value> _proto = v8::Handle<v8::Value>());
    

    std::string name() const { return mName; }

    bool hasValue() const { return !mValue.IsEmpty(); }
    v8::Handle<v8::Value> value() const { assert(hasValue()); return mValue; }

    bool hasPrototype() const { return !mPrototype.IsEmpty(); }
    v8::Handle<v8::Value> prototype() const { assert(hasPrototype()); return mPrototype; }

    String toString() const { return "[Pattern]"; }

    bool matches(v8::Handle<v8::Object> obj) const;
    void printPattern() const;
    
private:
    std::string mName;
    v8::Persistent<v8::Value> mValue;
    v8::Persistent<v8::Value> mPrototype;
};

typedef std::vector<Pattern> PatternList;

}//namespace js
}//namespace sirikata


#endif
