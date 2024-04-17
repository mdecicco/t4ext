#include <core/CGame.h>
#include <core/CLevel.h>
#include <script/IScriptAPI.hpp>
#include <events/Update.h>

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

    u32 CGame::addUpdateListener(t4ext::ICallback<void>& callback) {
        return utils::Singleton<EngineUpdateEventType>::Get()->createListener(callback);
    }

    void CGame::removeUpdateListener(u32 id) {
        utils::Singleton<EngineUpdateEventType>::Get()->removeListener(id);
    }

    u32 CGame::addRenderListener(t4ext::ICallback<void>& callback) {
        return utils::Singleton<EngineRenderEventType>::Get()->createListener(callback);
    }

    void CGame::removeRenderListener(u32 id) {
        utils::Singleton<EngineRenderEventType>::Get()->removeListener(id);
    }

    void CGame::disableInput() {
        DWORD dwBack;
        VirtualProtect((BYTE*)0x004D0466, 1, PAGE_EXECUTE_READWRITE, &dwBack);
        *(BYTE*)(0x004D0466) = 0xEB;
    }

    void CGame::enableInput() {
        DWORD dwBack;
        VirtualProtect((BYTE*)0x004D0466, 1, PAGE_EXECUTE_READWRITE, &dwBack);
        *(BYTE*)(0x004D0466) = 0x74;
    }
};