#include <Client.h>
#include <core/CActor.h>
#include <core/CGame.h>
#include <core/CLevel.h>

#include <script/IScriptAPI.hpp>
#include <script/TypeScriptAPI.h>
#include <script/Bindings.h>

#include <events/IEvent.h>
#include <events/Timeout.h>

#include <utils/Singleton.hpp>

#include <MinHook.h>
#include <windows.h>
#include <stdarg.h>

namespace t4ext {
    undefined4 (*FUN_005c0760_orig)(const char* fmt, ...) = nullptr;
    undefined4 FUN_005c0760_detour(const char* fmt, ...) {
        printf("[Game] ");
        va_list ap;
        va_start(ap, fmt);
        i32 count = vprintf(fmt, ap);
        va_end(ap);

        return count;
    }

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

    Client::Client() : utils::IWithLogging("Client") {
        // just for me
        AllocConsole();
        freopen("CONOUT$", "w", stdout);
        HWND consoleWindow = FindWindowA("ConsoleWindowClass", NULL);
        SetWindowPos(consoleWindow, 0, -1200, 300, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

        m_logFile = nullptr;
        fopen_s(&m_logFile, "C:\\Users\\miguel\\programming\\t4ext\\t4ext.log", "w");

        m_currentLevel = nullptr;
        m_scriptAPI = nullptr;

        m_runTime.start();
    }

    Client::~Client() {
        utils::Singleton<TimeoutEventType>::Destroy();
        utils::Singleton<IEventType>::Destroy();

        m_scriptAPI->shutdown();

        delete m_scriptAPI;
        m_scriptAPI = nullptr;

        if (MH_Uninitialize() != MH_OK) error("Failed to shutdown minhook");

        if (m_logFile) fclose(m_logFile);
        m_logFile = nullptr;
    }

    bool Client::init() {
        if (MH_Initialize() != MH_OK) {
            error("Failed to initialize minhook");
            return false;
        }

        m_scriptAPI = new TypeScriptAPI();
        m_scriptAPI->subscribeLogger(this);
        if (!m_scriptAPI->initPaths()) return false;
        
        BindAPI(m_scriptAPI);

        utils::Singleton<IEventType>::Create();
        utils::Singleton<TimeoutEventType>::Create();
        
        m_scriptAPI->registerEventType<IEvent>(utils::Singleton<IEventType>::Get(), "Event");
        m_scriptAPI->registerEventType<TimeoutEvent>(utils::Singleton<TimeoutEventType>::Get(), "TimeoutEvent");

        m_scriptAPI->commitBindings();

        m_clientThread.reset([](){ utils::Singleton<Client>::Get()->run(); });
        if (m_clientThread.getId() == 0) {
            error("Failed to create client thread");
            return false;
        }
        
        return true;
    }

    void Client::run() {
        try {
            // redirect game's own debug logs
            MH_CreateHook((LPVOID)0x005c0760, (LPVOID)&FUN_005c0760_detour, (LPVOID*)&FUN_005c0760_orig);
            MH_EnableHook((LPVOID)0x005c0760);

            // Hook actor creation to get notifications about it
            MH_CreateHook((LPVOID)0x00481510, (LPVOID)&CreateActor, (LPVOID*)&CreateActorOrig);
            MH_EnableHook((LPVOID)0x00481510);

            log("Client thread started");

            if (!m_scriptAPI->executeEntry()) {
                MH_RemoveHook((LPVOID)0x00481510);
                MH_RemoveHook((LPVOID)0x005c0760);
                log("Client thread exiting");
                return;
            }

            MH_RemoveHook((LPVOID)0x00481510);
            MH_RemoveHook((LPVOID)0x005c0760);
        } catch (const std::exception& e) {
            log("Caught exception: %s", e.what());
        }

        log("Client thread exiting");
    }

    f32 Client::elapsedTime() {
        return m_runTime.elapsed();
    }

    CLevel* Client::currentLevel() {
        return m_currentLevel;
    }

    void Client::onActorCreated(CLevel* lvl, CActor* actor, const char* name, const char* type, const char* model) {
        // log("onActorCreated(0x%X, '%s', '%s')", lvl, actor->getName(), name);
        if (lvl != m_currentLevel) {
            log("Level change detected: 0x%X -> 0x%X (%s | 0x%X)", m_currentLevel, lvl, lvl->atrPath, lvl->atrPath);
        }

        m_currentLevel = lvl;
    }

    void Client::onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) {
        propagateLog(level, scope, message);

        utils::String msg = utils::String::Format("[%s] %s", scope.c_str(), message.c_str());

        printf("%s\n", msg.c_str());
        fflush(stdout);

        if (m_logFile) {
            fprintf(m_logFile, "%s\n", msg.c_str());
            fflush(m_logFile);
        }
    }
};