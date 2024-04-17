#pragma once
#include <types.h>
#include <core/AppBase.h>

namespace t4ext {
    // This object is apparently 0x4645c in size???
    // That can't be right
    //
    // Also, there are a few other classes in the inheritance chain between these two
    class Application : public AppBase {
        public:
    };
};