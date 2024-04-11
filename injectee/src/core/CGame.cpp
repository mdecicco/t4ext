#include <core/CGame.h>

#include <string.h>

namespace t4ext {
    CGame* CGame::Get() {
        return *(CGame**)0x006b52b4;
    }

    CActor* CGame::spawnActorOverride(CLevel* level, const char* name, const char* type, const char* mtfPath) {
        u32 addr = 0x00481510;
        CActor* (CGame::*fn)(CLevel*, const char*, const char*, const char*);
        memcpy(&fn, &addr, 4);
        return (this->*fn)(level, name, type, mtfPath);
    }
};