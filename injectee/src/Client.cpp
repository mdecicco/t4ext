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

#include <utils/Singleton.hpp>
#include <hooks/Hooks.h>

#include <MinHook.h>
#include <imgui.h>
#include <backends/imgui_impl_dx9.h>
#include <backends/imgui_impl_win32.h>

#include <stdarg.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace t4ext {
    Client::Client() : utils::IWithLogging("Client") {
        // freopen("CONOUT$", "w", stdout);

        m_logFile = nullptr;
        fopen_s(&m_logFile, "C:\\Users\\miguel\\programming\\t4ext\\t4ext.log", "w");

        m_scriptAPI = nullptr;

        m_runTime.start();
        m_scriptAPI = new TypeScriptAPI();
    }

    Client::~Client() {
        log("Signaling the script thread to break the event loop and exit gracefully");
        m_scriptAPI->signalTermination();
        m_scriptThread.waitForExit();

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
        
        return true;
    }

    void Client::run() {
        Sleep(2000);

        m_scriptAPI->subscribeLogger(this);
        if (!m_scriptAPI->initPaths()) return;
        
        m_scriptAPI->initialize();
        BindAPI(m_scriptAPI);

        utils::Singleton<IEventType>::Create();
        utils::Singleton<TimeoutEventType>::Create();
        utils::Singleton<EngineUpdateEventType>::Create();
        utils::Singleton<EngineRenderEventType>::Create();

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

        utils::Singleton<EngineRenderEventType>::Destroy();
        utils::Singleton<EngineUpdateEventType>::Destroy();
        utils::Singleton<TimeoutEventType>::Destroy();
        utils::Singleton<IEventType>::Destroy();

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
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui::SetMouseCursor(ImGuiMouseCursor_::ImGuiMouseCursor_Arrow);
        ImGui_ImplWin32_Init(m_gameWindow);
    }
    
    void Client::onPeekMessage(LPMSG msg, HWND window) {
        ImGui_ImplWin32_WndProcHandler(window, msg->message, msg->wParam, msg->lParam);
    }
    
    void Client::onBeforeUpdate() {
        if (!m_scriptAPI->isReady()) return;
        m_scriptAPI->dispatchEvent(utils::Singleton<EngineUpdateEventType>::Get()->createEvent());

        m_scriptAPI->signalEventBatchStart();
        m_scriptAPI->waitForEventBatchCompletion();
    }

    void Client::onBeforeRender() {
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
    
    void Client::onActorCreated(CLevel* lvl, CActor* actor, const char* name, const char* type, const char* model) {
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