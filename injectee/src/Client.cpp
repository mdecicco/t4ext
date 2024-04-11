#include <Client.h>
#include <core/CActor.h>
#include <core/CGame.h>
#include <core/CLevel.h>

#include <utils/Singleton.hpp>
#include <MinHook.h>

#include <windows.h>

namespace t4ext {
    undefined4 (*FUN_005c0760)(const char* fmt, ...) = nullptr;
    class CLevel;
    class CGame;
    CActor* (CGame::*CreateActorOrig)(CLevel*, const char*, const char*, const char*);
    CActor* __fastcall CreateActor(CGame* game, void * _EDX, CLevel* level, const char* name, const char* type, const char* mtfPath) {
        Client* c = utils::Singleton<Client>::Get();
        // c->log("CreateActor('%s', '%s', '%s')", name, type, mtfPath);

        CActor* actor = (game->*CreateActorOrig)(level, name, type, mtfPath);
        c->onActorCreated(level, actor, name, type, mtfPath);
        return actor;
    }

    void ClientThread() {
        utils::Singleton<Client>::Get()->run();
    }

    Client::Client() : utils::IWithLogging("Client") {
        m_logFile = nullptr;
        fopen_s(&m_logFile, "C:\\Users\\miguel\\programming\\t4ext\\t4ext.log", "w");

        m_currentLevel = nullptr;
    }

    Client::~Client() {
        if (MH_Uninitialize() != MH_OK) error("Failed to shutdown minhook");

        if (m_logFile) fclose(m_logFile);
        m_logFile = nullptr;
    }

    bool Client::init() {
        if (MH_Initialize() != MH_OK) {
            error("Failed to initialize minhook");
            return false;
        }

        m_clientThread.reset(ClientThread);
        if (m_clientThread.getId() == 0) {
            error("Failed to create client thread");
            return false;
        }

        return true;
    }

    static u32 screenChangeCount = 0;

    void Client::run() {
        Sleep(5000);
        FUN_005c0760 = (undefined4(*)(const char*, ...))0x005c0760;
        log("Client thread started");

        MH_CreateHook((LPVOID)0x00481510, (LPVOID)&CreateActor, (LPVOID*)&CreateActorOrig);
        MH_EnableHook((LPVOID)0x00481510);

        while (true) {
            Sleep(1000);
            if (m_currentLevel && screenChangeCount >= 2) {
                /*
                CActor* actor = CGame::Get()->spawnActorOverride(
                    m_currentLevel,
                    "Y:\\Data\\Actors\\EnemyVariations\\Soldier\\BarracksSoldierRifle.atr",
                    "HumanAI", 
                    "$/\\Data\\Actors\\Enemies\\Dinosoids\\Soldier\\nme_bb_troopsoid_model.atf"
                );

                actor->setPosition({ -2.0f, -3.0f, -20.0f });
                actor->setVisibility(true);
                actor->setCollides(3);
                actor->setTouches(0x400);
                actor->setIgnores(0);
                m_currentLevel->spawnActor(actor, 0);
                */

               m_currentLevel->spawnActorAtPosition(0, "HumanAI", "Y:\\Data\\Actors\\EnemyVariations\\Soldier\\BarracksSoldierRifle.atr", { -2.0f, -5.0f, 20.0f }, 0);
            }
        }

        MH_RemoveHook((LPVOID)0x00481510);
    }

    void Client::onActorCreated(CLevel* lvl, CActor* actor, const char* name, const char* type, const char* model) {
        log("onActorCreated(0x%X, '%s', '%s')", lvl, actor->getName(), name);
        if (lvl != m_currentLevel) {
            screenChangeCount++;
            log("Level change detected: 0x%X -> 0x%X", m_currentLevel, lvl);
        }

        m_currentLevel = lvl;
    }

    void Client::onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) {
        propagateLog(level, scope, message);

        utils::String msg = scope + ": " + message;

        if (FUN_005c0760) FUN_005c0760("%s\n", msg.c_str());
        if (m_logFile) {
            fprintf(m_logFile, "%s\n", msg.c_str());
            fflush(m_logFile);
        }
    }
};