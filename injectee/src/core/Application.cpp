#include <core/Application.h>

namespace t4ext {
    Application* Application::Get() {
        return *(Application**)0x006befb0;
    }
};