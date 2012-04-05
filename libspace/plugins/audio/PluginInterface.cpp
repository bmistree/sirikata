// Copyright (c) 2011 Sirikata Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file.

#include <sirikata/space/SpaceModule.hpp>
#include "Audio.hpp"
#include <sirikata/core/options/Options.hpp>

static int space_audio_plugin_refcount = 0;

namespace Sirikata {

static void InitPluginOptions() {
    Sirikata::InitializeClassOptions ico("space_audio", NULL,
        NULL);
}

static SpaceModule* createAudio(SpaceContext* ctx, const String& args) {
    OptionSet* optionsSet = OptionSet::getOptions("space_audio",NULL);
    optionsSet->parse(args);

    return new Audio(ctx);
}

} // namespace Sirikata

SIRIKATA_PLUGIN_EXPORT_C void init() {
    using namespace Sirikata;
    if (space_audio_plugin_refcount==0) {
        InitPluginOptions();
        using std::tr1::placeholders::_1;
        using std::tr1::placeholders::_2;
        using std::tr1::placeholders::_3;
        using std::tr1::placeholders::_4;
        using std::tr1::placeholders::_5;
        SpaceModuleFactory::getSingleton()
            .registerConstructor("audio",
                std::tr1::bind(&createAudio, _1, _2));
    }
    space_audio_plugin_refcount++;
}

SIRIKATA_PLUGIN_EXPORT_C int increfcount() {
    return ++space_audio_plugin_refcount;
}
SIRIKATA_PLUGIN_EXPORT_C int decrefcount() {
    assert(space_audio_plugin_refcount>0);
    return --space_audio_plugin_refcount;
}

SIRIKATA_PLUGIN_EXPORT_C void destroy() {
    using namespace Sirikata;
    if (space_audio_plugin_refcount==0) {
        SpaceModuleFactory::getSingleton().unregisterConstructor("audio");
    }
}

SIRIKATA_PLUGIN_EXPORT_C const char* name() {
    return "audio";
}

SIRIKATA_PLUGIN_EXPORT_C int refcount() {
    return space_audio_plugin_refcount;
}
