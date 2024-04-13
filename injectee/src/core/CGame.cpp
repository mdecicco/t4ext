#include <core/CGame.h>
#include <core/CLevel.h>

#include <utils/Array.hpp>
#include <utils/Singleton.hpp>
#include <Client.h>

#include <string.h>

namespace t4ext {
    CGame* CGame::Get() {
        return *(CGame**)0x006b52b4;
    }

    CActor* CGame::spawnActorOverride(const char* name, const char* type, const char* mtfPath) {
        CLevel* level = gClient::Get()->currentLevel();
        if (!level) return nullptr;
        u32 addr = 0x00481510;
        CActor* (CGame::*fn)(CLevel*, const char*, const char*, const char*);
        memcpy(&fn, &addr, 4);
        return (this->*fn)(level, name, type, mtfPath);
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
        return gClient::Get()->currentLevel();
    }
};