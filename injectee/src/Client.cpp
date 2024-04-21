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
#include <events/LevelDestroy.h>
#include <events/Log.h>
#include <events/Keyboard.h>

#include <utils/Singleton.hpp>
#include <utils/Input.h>

#include <hooks/Hooks.h>

#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_dx9.h>
#include <backends/imgui_impl_win32.h>
#include <Windowsx.h>
#include <stdarg.h>
#include <core/Application.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace utils {
    bool isKeyCodeValid(u32 code);
};

namespace t4ext {
    static bool keyDown[512] = { false };

    Client::Client() : utils::IWithLogging("Client") {
        // freopen("CONOUT$", "w", stdout);

        m_logFile = nullptr;
        fopen_s(&m_logFile, "C:\\Users\\miguel\\programming\\t4ext\\t4ext.log", "w");

        m_scriptAPI = nullptr;

        m_runTime.start();
        m_scriptAPI = new TypeScriptAPI();

        m_alreadyPeekedThisFrame = false;
    }

    Client::~Client() {
        log("Signaling the script thread to break the event loop and exit gracefully");
        m_scriptAPI->signalTermination();
        m_scriptThread.waitForExit();

        utils::Singleton<LogEventType>::Destroy();
        utils::Singleton<KeyboardEventType>::Destroy();
        utils::Singleton<ActorCreateEventType>::Destroy();
        utils::Singleton<ActorDestroyEventType>::Destroy();
        utils::Singleton<LevelCreateEventType>::Destroy();
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
        utils::Singleton<LevelCreateEventType>::Create();
        utils::Singleton<ActorDestroyEventType>::Create();
        utils::Singleton<ActorCreateEventType>::Create();
        utils::Singleton<KeyboardEventType>::Create();
        utils::Singleton<LogEventType>::Create();

        return true;
    }

    void Client::run() {
        m_scriptAPI->subscribeLogger(this);
        if (!m_scriptAPI->initPaths()) return;
        
        m_scriptAPI->initialize();

        BindAPI(m_scriptAPI);
        utils::Singleton<LogEventType>::Get()->bind();
        utils::Singleton<KeyboardEventType>::Get()->bind();

        m_scriptAPI->commitBindings();
        try {
            log("Client thread started");

            // This will start the "event loop" which will more or less
            // loop infinitely, which is why this needs to be in its own thread.
            //
            // The script entrypoint itself will just perform some basic initialization
            // tasks, like setting up event handlers and whatnot and it'll complete relatively
            // quickly I assume. It's after that that the event loop begins and it just waits
            // for events to occur.
            //
            // Events will be produced by things like timeouts, network request responses,
            // promise resolutions, but also by the game itself.
            //
            // The event loop should also wait until we receive control from the game at key
            // points. We will hook the game's top-level update and render functions, or anything
            // that works well enough. At those points we will signal the event loop to process
            // the next batch of events and wait for it to signal back that it's okay for the game
            // to continue executing. The event loop should be limited in the amount of time that
            // it spends on processing so it does not interfere with the game's performance too
            // much.

            m_scriptAPI->executeEntry();

        } catch (const std::exception& e) {
            log("Caught exception: %s", e.what());
        }

        // todo:
        // fix the crash that occurrs in the main thread if the script thread exits

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
    }
    
    void Client::onPeekMessage(LPMSG msg, HWND window) {
        if (!m_scriptAPI->isReady()) return;

        // for some reason peek message is getting called 3 times per frame...
        if (m_alreadyPeekedThisFrame) return;
        m_alreadyPeekedThisFrame = true;
        ImGui_ImplWin32_WndProcHandler(window, msg->message, msg->wParam, msg->lParam);

        switch (msg->message) {
            case WM_KEYDOWN: {
                utils::KeyboardKey code = utils::KeyboardKey(HIWORD(msg->lParam) & (KF_EXTENDED | 0xff));
                if (code == utils::KEY_NONE) code = utils::KeyboardKey(MapVirtualKeyW(u32(msg->wParam), MAPVK_VK_TO_VSC));
                if (!isKeyCodeValid(code)) break;
                
                if (!keyDown[code]) m_scriptAPI->dispatchEvent(utils::Singleton<KeyboardEventType>::Get()->createEvent(code, KeyboardEvent::KeyState::Pressed));
                keyDown[code] = true;

                break;
            }
            case WM_KEYUP: {
                utils::KeyboardKey code = utils::KeyboardKey(HIWORD(msg->lParam) & (KF_EXTENDED | 0xff));
                if (code == utils::KEY_NONE) code = utils::KeyboardKey(MapVirtualKeyW(u32(msg->wParam), MAPVK_VK_TO_VSC));
                if (!isKeyCodeValid(code)) break;

                keyDown[code] = false;
                m_scriptAPI->dispatchEvent(utils::Singleton<KeyboardEventType>::Get()->createEvent(code, KeyboardEvent::KeyState::Released));

                break;
            }
            case WM_MOUSEMOVE: {
                u32 x = GET_X_LPARAM(msg->lParam);
                u32 y = GET_Y_LPARAM(msg->lParam);

                // todo
                break;
            }
            case WM_MOUSEWHEEL: {
                i16 x = HIWORD(msg->wParam);
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
        m_alreadyPeekedThisFrame = false;
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (m_scriptAPI->isReady()) {
            m_scriptAPI->dispatchEvent(utils::Singleton<EngineRenderEventType>::Get()->createEvent());

            m_scriptAPI->signalEventBatchStart();
            m_scriptAPI->waitForEventBatchCompletion();
        }

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
        m_scriptAPI->dispatchEvent(utils::Singleton<ActorCreateEventType>::Get()->createEvent(actor));
    }
    
    void Client::onActorDestroyed(CActor* actor) {
        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<ActorDestroyEventType>::Get()->createEvent(actor));

        // Destruction of the actor is imminent, sorry! We must execute the event handlers now
        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onLevelCreated(CLevel* level) {
        m_scriptAPI->dispatchEvent(utils::Singleton<LevelCreateEventType>::Get()->createEvent(level));
    }

    void Client::onLevelDestroyed(CLevel* level) {
        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<LevelDestroyEventType>::Get()->createEvent(level));

        // Destruction of the level is imminent, sorry! We must execute the event handlers now
        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) {
        propagateLog(level, scope, message);

        m_scriptAPI->dispatchEvent(utils::Singleton<LogEventType>::Get()->createEvent(level, scope, message));

        utils::String msg = utils::String::Format("[%s] %s", scope.c_str(), message.c_str());

        printf("%s\n", msg.c_str());
        fflush(stdout);

        if (m_logFile) {
            fprintf(m_logFile, "%s\n", msg.c_str());
            fflush(m_logFile);
        }
    }
};