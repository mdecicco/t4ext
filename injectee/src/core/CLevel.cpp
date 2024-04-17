#include <core/CLevel.h>
#include <utils/Singleton.hpp>
#include <Client.h>

#include <string.h>

namespace t4ext {
    CActor* CLevel::createActor(const char* type, const char* path) {
        u32 addr = 0x0050dee0;
        CActor* (__thiscall CLevel::*fn)(const char*, const char*);
        memcpy(&fn, &addr, 4);
        return (this->*fn)(type, path);
    }
    
    CActor* CLevel::addActor(CActor* actor, i32 p2) {
        u32 addr = 0x00511410;
        CActor* (__thiscall CLevel::*fn)(CActor*, i32);
        memcpy(&fn, &addr, 4);
        return (this->*fn)(actor, p2);
    }
    
    CActor* CLevel::spawnActor(CActor* actor, i32 p2) {
        u32 addr = 0x00511560;
        CActor* (__thiscall CLevel::*fn)(CActor*, i32);
        memcpy(&fn, &addr, 4);
        return (this->*fn)(actor, p2);
    }
    
    CActor* CLevel::spawnActorAtPosition(i32 p1, const char* type, const char* path, const utils::vec3f& pos, i32 p5) {
        u32 addr = 0x005120e0;
        CActor* (__thiscall CLevel::*fn)(i32, const char*, const char*, const utils::vec3f&, i32);
        memcpy(&fn, &addr, 4);
        return (this->*fn)(p1, type, path, pos, p5);
    }
};