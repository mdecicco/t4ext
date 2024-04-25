#include <core/CGame.h>
#include <core/CLevel.h>
#include <script/IScriptAPI.hpp>
#include <events/Update.h>
#include <events/ActorCreate.h>
#include <events/ActorDestroy.h>
#include <events/LevelCreate.h>
#include <events/LevelSpawn.h>
#include <events/LevelDestroy.h>

#include <utils/Array.hpp>
#include <utils/Singleton.hpp>
#include <Client.h>

#include <string.h>

namespace t4ext {
    CGame* CGame::Get() {
        return *(CGame**)0x006b52b4;
    }
    
    utils::Array<CLevel*>* CGame::getLevels() {
        if (levels.size() == 0) return nullptr;

        utils::Array<CLevel*>* ret = new utils::Array<CLevel*>();

        u32 sz = levels.size();
        for (u32 i = 0;i < sz;i++) {
            ret->push(levels[i]);
        }

        return ret;
    }
    
    CLevel* CGame::getCurrentLevel() {
        if (levels.size() == 0) return nullptr;

        // todo: this is just an assumption
        return levels[0];
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

    u32 CGame::addLevelSpawnListener(t4ext::Callback<void, CLevel*>& callback) {
        return utils::Singleton<LevelSpawnEventType>::Get()->createListener(callback);
    }

    void CGame::removeLevelSpawnListener(u32 id) {
        utils::Singleton<LevelSpawnEventType>::Get()->removeListener(id);
    }

    void CGame::disableInput() {
        gClient::Get()->setGameInputDisabled(true);
    }

    void CGame::enableInput() {
        gClient::Get()->setGameInputDisabled(false);
    }
};