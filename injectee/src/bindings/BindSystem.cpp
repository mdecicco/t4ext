#include <bindings/Bindings.h>
#include <script/IScriptAPI.hpp>

#include <filesystem>
#include <windows.h>

namespace t4ext {
    void BindSystem(IScriptAPI* api) {
        api->bind("loadDLL", +[](const char* path){
            HMODULE mod = LoadLibraryA(path);
            if (!mod) return false;

            CloseHandle(mod);
            return true;
        });
    }
};
            