#include <Client.h>
#include <hooks/d3d.h>

#include <core/CActor.h>
#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/Application.h>

#include <script/IScriptAPI.hpp>
#include <script/TypeScriptAPI.h>
#include <bindings/Bindings.h>

#include <events/IEvent.h>
#include <events/Timeout.h>
#include <events/Update.h>
#include <events/EngineCreate.h>
#include <events/ActorCreate.h>
#include <events/ActorDestroy.h>
#include <events/LevelCreate.h>
#include <events/LevelSpawn.h>
#include <events/LevelDestroy.h>
#include <events/Log.h>
#include <events/Keyboard.h>
#include <events/ActorCollision.h>
#include <events/ActorAdded.h>

#include <utils/Singleton.hpp>
#include <utils/Input.h>

#include <hooks/Hooks.h>

#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_dx9.h>
#include <backends/imgui_impl_win32.h>

#include <psapi.h>
#include <Windowsx.h>
#include <stdarg.h>
#include <filesystem>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace utils {
    bool isKeyCodeValid(u32 code);
};

namespace t4ext {
    static bool keyDown[512] = { false };
    static ImFont* font = nullptr;

    Client::Client() : utils::IWithLogging("Client"), m_gameLogs("Game") {
        m_gameLogs.subscribeLogger(this);
        m_logFile = nullptr;

        char procPath[MAX_PATH] = { 0 };
        if (GetModuleFileNameEx(GetCurrentProcess(), nullptr, procPath, MAX_PATH) != 0) {
            utils::String pathBase = std::filesystem::u8path(procPath).remove_filename().string();
            utils::String logPath;
            
            pathBase.replaceAll('\\', '/');
            if (pathBase.size() > 0 && pathBase[pathBase.size() - 1] == '/') {
                logPath = pathBase + "t4ext.log";
            } else {
                logPath = pathBase + "/t4ext.log";
            }
            
            fopen_s(&m_logFile, logPath.c_str(), "w");
        } else {
            error("Failed to get process path, not opening log file...");
        }

        m_scriptAPI = nullptr;

        m_runTime.start();
        m_scriptAPI = new TypeScriptAPI();

        m_gameInputDisabled = false;
    }

    Client::~Client() {
        log("Signaling the script thread to break the event loop and exit gracefully");
        m_scriptAPI->signalTermination();
        m_scriptThread.waitForExit();

        utils::Singleton<ActorAddedEventType>::Destroy();
        utils::Singleton<ActorCollisionEventType>::Destroy();
        utils::Singleton<LogEventType>::Destroy();
        utils::Singleton<KeyboardEventType>::Destroy();
        utils::Singleton<ActorCreateEventType>::Destroy();
        utils::Singleton<ActorDestroyEventType>::Destroy();
        utils::Singleton<LevelCreateEventType>::Destroy();
        utils::Singleton<LevelSpawnEventType>::Destroy();
        utils::Singleton<LevelDestroyEventType>::Destroy();
        utils::Singleton<EngineCreateEventType>::Destroy();
        utils::Singleton<EngineRenderEventType>::Destroy();
        utils::Singleton<EngineUpdateEventType>::Destroy();
        utils::Singleton<TimeoutEventType>::Destroy();
        utils::Singleton<IEventType>::Destroy();

        delete m_scriptAPI;
        m_scriptAPI = nullptr;

        UninstallHooks();

        if (MH_Uninitialize() != MH_OK) error("Failed to shutdown minhook");

        if (m_logFile) fclose(m_logFile);
        m_logFile = nullptr;
    }

    bool Client::init() {
        if (MH_Initialize() != MH_OK) {
            error("Failed to initialize minhook");
            return false;
        }

        m_scriptThread.reset([](){ utils::Singleton<Client>::Get()->run(); });
        if (m_scriptThread.getId() == 0) {
            error("Failed to create script thread");
            return false;
        }
        
        InstallHooks();
        
        utils::Singleton<IEventType>::Create();
        utils::Singleton<TimeoutEventType>::Create();
        utils::Singleton<EngineUpdateEventType>::Create();
        utils::Singleton<EngineRenderEventType>::Create();
        utils::Singleton<EngineCreateEventType>::Create();
        utils::Singleton<LevelDestroyEventType>::Create();
        utils::Singleton<LevelSpawnEventType>::Create();
        utils::Singleton<LevelCreateEventType>::Create();
        utils::Singleton<ActorDestroyEventType>::Create();
        utils::Singleton<ActorCreateEventType>::Create();
        utils::Singleton<KeyboardEventType>::Create();
        utils::Singleton<LogEventType>::Create();
        utils::Singleton<ActorCollisionEventType>::Create();
        utils::Singleton<ActorAddedEventType>::Create();

        return true;
    }

    void Client::run() {
        log("Client thread started");

        m_scriptAPI->subscribeLogger(this);
        if (!m_scriptAPI->initPaths()) return;
        
        m_scriptAPI->initialize();

        BindAPI(m_scriptAPI);
        utils::Singleton<LogEventType>::Get()->bind();
        utils::Singleton<KeyboardEventType>::Get()->bind();

        m_scriptAPI->commitBindings();
        
        try {
            m_scriptAPI->executeEntry();
        } catch (const std::exception& e) {
            log("Caught exception: %s", e.what());
        }

        bool doExit = false;
        while (!doExit) {
            try {
                doExit = m_scriptAPI->handleEvents();
            } catch (const std::exception& e) {
                log("Caught exception: %s", e.what());
            }
        }

        m_scriptAPI->shutdown();

        log("Client thread exiting");
    }

    f32 Client::elapsedTime() {
        return m_runTime.elapsed();
    }

    IScriptAPI* Client::getAPI() {
        return m_scriptAPI;
    }
    
    void Client::onWindowCreated(HWND window) {
        m_gameWindow = window;

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();
        ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Arrow);
        ImGui_ImplWin32_Init(m_gameWindow);
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.7f;

        font = io.Fonts->AddFontFromFileTTF((m_scriptAPI->getGameBasePath() + "/arial.ttf").c_str(), 14.0f, nullptr, io.Fonts->GetGlyphRangesDefault());
    }
    
    void Client::onWindowProc(HWND window, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        if (!m_scriptAPI->isReady()) return;

        ImGui_ImplWin32_WndProcHandler(window, uMsg, wParam, lParam);

        switch (uMsg) {
            case WM_KEYDOWN: {
                utils::KeyboardKey code = utils::KeyboardKey(HIWORD(lParam) & (KF_EXTENDED | 0xff));
                if (code == utils::KEY_NONE) code = utils::KeyboardKey(MapVirtualKeyW(u32(wParam), MAPVK_VK_TO_VSC));
                if (!isKeyCodeValid(code)) break;
                
                if (!keyDown[code]) m_scriptAPI->dispatchEvent(utils::Singleton<KeyboardEventType>::Get()->createEvent(code, KeyboardEvent::KeyState::Pressed));
                keyDown[code] = true;

                break;
            }
            case WM_KEYUP: {
                utils::KeyboardKey code = utils::KeyboardKey(HIWORD(lParam) & (KF_EXTENDED | 0xff));
                if (code == utils::KEY_NONE) code = utils::KeyboardKey(MapVirtualKeyW(u32(wParam), MAPVK_VK_TO_VSC));
                if (!isKeyCodeValid(code)) break;

                keyDown[code] = false;
                m_scriptAPI->dispatchEvent(utils::Singleton<KeyboardEventType>::Get()->createEvent(code, KeyboardEvent::KeyState::Released));

                break;
            }
            case WM_MOUSEMOVE: {
                u32 x = GET_X_LPARAM(lParam);
                u32 y = GET_Y_LPARAM(lParam);

                // todo
                break;
            }
            case WM_MOUSEWHEEL: {
                i16 x = HIWORD(wParam);
                f32 d = x / f32(WHEEL_DELTA);

                // todo
                break;
            }
            case WM_LBUTTONDOWN: {
                // todo utils::LEFT_BUTTON down
                break;
            }
            case WM_LBUTTONUP: {
                // todo utils::LEFT_BUTTON Up
                break;
            }
            case WM_MBUTTONDOWN: {
                // todo utils::MIDDLE_BUTTON Down
                break;
            }
            case WM_MBUTTONUP: {
                // todo utils::MIDDLE_BUTTON Up
                break;
            }
            case WM_RBUTTONDOWN: {
                // todo utils::RIGHT_BUTTON Down
                break;
            }
            case WM_RBUTTONUP: {
                // todo utils::RIGHT_BUTTON Up
                break;
            }
        }
    }
    
    void Client::onBeforeUpdate() {
        if (!m_scriptAPI->isReady()) return;

        for (u32 code = 0;code < 512;code++) {
            if (keyDown[code]) {
                m_scriptAPI->dispatchEvent(utils::Singleton<KeyboardEventType>::Get()->createEvent(utils::KeyboardKey(code), KeyboardEvent::KeyState::Held));
            }
        }

        m_scriptAPI->dispatchEvent(utils::Singleton<EngineUpdateEventType>::Get()->createEvent());

        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onBeforeRender() {
        if (!m_scriptAPI->isReady()) return;

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGuiViewport* mvp = ImGui::GetMainViewport();
        mvp->WorkSize = mvp->Size = ImVec2(1920, 1080);
        ImGuiViewport* wvp = ImGui::GetWindowViewport();
        wvp->WorkSize = wvp->Size = ImVec2(1920, 1080);
        
        if (font) ImGui::PushFont(font);

        m_scriptAPI->dispatchEvent(utils::Singleton<EngineRenderEventType>::Get()->createEvent());

        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();

        if (font) ImGui::PopFont();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }

    void Client::onDeviceInit(d3dDevice* device) {
        ImGui_ImplDX9_Init(device);
    }
    
    void Client::onDeviceReset(d3dDevice* device) {
        ImGui_ImplDX9_InvalidateDeviceObjects();
    }
    
    void Client::onEngineCreated(CGame* engine) {
        m_scriptAPI->dispatchEvent(utils::Singleton<EngineCreateEventType>::Get()->createEvent(engine));
    }
    
    void Client::onActorCreated(CActor* actor) {
        m_liveActors.push(actor);

        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<ActorCreateEventType>::Get()->createEvent(actor));

        // Because actor destruction has to immediately invoke the scripts... So does actor creation
        // Otherwise short-lived actors may have the creation events fired _after_ the destruction
        // events
        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }
    
    void Client::onActorDestroyed(CActor* actor) {
        for (u32 i = 0;i < m_liveActors.size();i++) {
            if (m_liveActors[i] == actor) {
                m_liveActors.remove(i);
                break;
            }
        }

        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<ActorDestroyEventType>::Get()->createEvent(actor));

        // Destruction of the actor is imminent, sorry! We must execute the event handlers now
        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onActorAddedToLevel(CLevel* level, CActor* actor) {
        m_scriptAPI->dispatchEvent(utils::Singleton<ActorAddedEventType>::Get()->createEvent(level, actor));
    }

    void Client::onLevelCreated(CLevel* level) {
        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<LevelCreateEventType>::Get()->createEvent(level));

        // Because level destruction has to immediately invoke the scripts... So does level creation
        // Otherwise short-lived levels may have the creation events fired _after_ the destruction
        // events
        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onLevelSpawned(CLevel* level) {
        m_scriptAPI->dispatchEvent(utils::Singleton<LevelSpawnEventType>::Get()->createEvent(level));
    }

    void Client::onLevelDestroyed(CLevel* level) {
        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<LevelDestroyEventType>::Get()->createEvent(level));

        // Destruction of the level is imminent, sorry! We must execute the event handlers now
        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onActorCollision(CActor* actorA, CActor* actorB) {
        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<ActorCollisionEventType>::Get()->createEvent(actorA, actorB));
    }

    void Client::onGameLog(const char* msg) {
        m_gameLogs.log(msg);
    }

    bool Client::isGameInputDisabled() {
        return m_gameInputDisabled;
    }

    void Client::setGameInputDisabled(bool isDisabled) {
        m_gameInputDisabled = isDisabled;

        // The above flag blocks the game's WndProc function
        // which for some bizarre reason only works for blocking
        // input from the main menu... And only everything but the
        // arrow keys?
        //
        // I'll just keep doing this for now to stop the player from
        // moving and looking around when it shouldn't receive input
        if (isDisabled) {
            DWORD dwBack;
            VirtualProtect((BYTE*)0x004D0466, 1, PAGE_EXECUTE_READWRITE, &dwBack);
            *(BYTE*)(0x004D0466) = 0xEB;
        } else {
            DWORD dwBack;
            VirtualProtect((BYTE*)0x004D0466, 1, PAGE_EXECUTE_READWRITE, &dwBack);
            *(BYTE*)(0x004D0466) = 0x74;
        }
    }
    
    const utils::Array<CActor*>& Client::getLiveActors() {
        return m_liveActors;
    }

    void Client::onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) {
        propagateLog(level, scope, message);

        if (m_scriptAPI->isReady()) {
            m_scriptAPI->dispatchEvent(utils::Singleton<LogEventType>::Get()->createEvent(level, scope, message));
        }

        utils::String msg = utils::String::Format("[%s] %s", scope.c_str(), message.c_str());

        printf("%s\n", msg.c_str());
        fflush(stdout);

        if (m_logFile) {
            fprintf(m_logFile, "%s\n", msg.c_str());
            fflush(m_logFile);
        }
    }
};