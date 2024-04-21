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
            static Application* Get();

            void** field1_0x258;
            undefined4 padding[5];
            bool isWindowed;
            undefined padding1[0x45AC3];
            bool wantsFullScreen;
    };
};