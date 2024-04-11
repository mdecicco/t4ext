#include <types.h>
#include <Application.h>

#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

using namespace t4ext;

i32 main() {
    i32 result = 0;

    utils::Mem::Create();
    
    {
        Application app;
        if (!app.run()) result = -1;
    }

    utils::Mem::Destroy();

    return result;
}