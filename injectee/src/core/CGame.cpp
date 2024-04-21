#include <core/CGame.h>
#include <core/CLevel.h>
#include <script/IScriptAPI.hpp>
#include <events/Update.h>
#include <events/ActorCreate.h>
#include <events/ActorDestroy.h>
#include <events/LevelCreate.h>
#include <events/LevelDestroy.h>

#include <utils/Array.hpp>
#include <utils/Singleton.hpp>
#include <Client.h>

#include <string.h>

namespace t4ext {
    CGame* CGame::Get() {
        return *(CGame**)0x006b52b4;
    }
    
    utils::Array<CLevel*> CGame::getLevels() {
        utils::Array<CLevel*> ret;

        if (levelListBegin) {
            CLevel** l = levelListBegin;
            while (l != levelListEnd) {
                if (*l) {
                    ret.push(*l);
                }

                l++;
            }
        }

        return ret;
    }
    
    CLevel* CGame::getCurrentLevel() {
        return nullptr;
    }

    u32 CGame::addUpdateListener(t4ext::Callback<void>& callback) {
        return utils::Singleton<EngineUpdateEventType>::Get()->createListener(callback);
    }

    void CGame::removeUpdateListener(u32 id) {
        utils::Singleton<EngineUpdateEventType>::Get()->removeListener(id);
    }

    u32 CGame::addRenderListener(t4ext::Callback<void>& callback) {
        return utils::Singleton<EngineRenderEventType>::Get()->createListener(callback);
    }

    void CGame::removeRenderListener(u32 id) {
        utils::Singleton<EngineRenderEventType>::Get()->removeListener(id);
    }

    u32 CGame::addActorCreateListener(t4ext::Callback<void, CActor*>& callback) {
        return utils::Singleton<ActorCreateEventType>::Get()->createListener(callback);
    }

    void CGame::removeActorCreateListener(u32 id) {
        utils::Singleton<ActorCreateEventType>::Get()->removeListener(id);
    }

    u32 CGame::addActorDestroyListener(t4ext::Callback<void, CActor*>& callback) {
        return utils::Singleton<ActorDestroyEventType>::Get()->createListener(callback);
    }

    void CGame::removeActorDestroyListener(u32 id) {
        utils::Singleton<ActorDestroyEventType>::Get()->removeListener(id);
    }

    u32 CGame::addLevelCreateListener(t4ext::Callback<void, CLevel*>& callback) {
        return utils::Singleton<LevelCreateEventType>::Get()->createListener(callback);
    }

    void CGame::removeLevelCreateListener(u32 id) {
        utils::Singleton<LevelCreateEventType>::Get()->removeListener(id);
    }

    u32 CGame::addLevelDestroyListener(t4ext::Callback<void, CLevel*>& callback) {
        return utils::Singleton<LevelDestroyEventType>::Get()->createListener(callback);
    }

    void CGame::removeLevelDestroyListener(u32 id) {
        utils::Singleton<LevelDestroyEventType>::Get()->removeListener(id);
    }

    void CGame::disableInput() {
    }

    void CGame::enableInput() {
    }
};