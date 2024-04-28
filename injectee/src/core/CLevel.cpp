#include <core/CLevel.h>
#include <events/ActorAdded.h>
#include <Client.h>

#include <utils/Singleton.hpp>
#include <utils/Array.hpp>

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

    utils::Array<CCamera*>* CLevel::getCameras() {
        u32 sz = cameras.size();
        if (sz == 0) return nullptr;
        
        utils::Array<CCamera*>* arr = new utils::Array<CCamera*>();

        for (u32 i = 0;i < sz;i++) arr->push(cameras[i]);

        return arr;
    }

    utils::Array<CActor*>* CLevel::getUpdateActors() {
        u32 sz = updateActors.size();
        if (sz == 0) return nullptr;
        
        utils::Array<CActor*>* arr = new utils::Array<CActor*>();

        for (u32 i = 0;i < sz;i++) arr->push(updateActors[i]);

        return arr;
    }

    utils::Array<CActor*>* CLevel::getActors() {
        u32 sz = actorVec.size();
        if (sz == 0) return nullptr;
        
        utils::Array<CActor*>* arr = new utils::Array<CActor*>();

        for (u32 i = 0;i < sz;i++) arr->push(actorVec[i]);

        return arr;
    }

    u32 CLevel::addActorAddedListener(Callback<void, CActor*>& cb) {
        return utils::Singleton<ActorAddedEventType>::Get()->createListener(this, cb);
    }

    void CLevel::removeActorAddedListener(u32 listenerId) {
        utils::Singleton<ActorAddedEventType>::Get()->removeListener(listenerId);
    }
};