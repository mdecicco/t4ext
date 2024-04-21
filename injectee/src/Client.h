#pragma once
#include <types.h>
#include <utils/ILogListener.h>
#include <utils/Thread.h>
#include <utils/Timer.h>

#include <stdio.h>

#define NOMINMAX
#include <windows.h>

namespace t4ext {
    class d3dDevice;
    class CGame;
    class CActor;
    class CLevel;
    class IScriptAPI;
    class TypeScriptAPI;

    class Client : public utils::IWithLogging {
        public:
            Client();
            virtual ~Client();

            bool init();
            void run();
            f32 elapsedTime();
            IScriptAPI* getAPI();

            void onWindowCreated(HWND window);
            void onPeekMessage(LPMSG msg, HWND window);
            void onBeforeUpdate();
            void onBeforeRender();
            void onDeviceInit(d3dDevice* device);
            void onDeviceReset(d3dDevice* device);
            void onEngineCreated(CGame* engine);
            void onActorCreated(CActor* actor);
            void onActorDestroyed(CActor* actor);
            void onLevelCreated(CLevel* level);
            void onLevelDestroyed(CLevel* level);

            virtual void onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message);
    
        protected:
            FILE* m_logFile;
            HWND m_gameWindow;
            TypeScriptAPI* m_scriptAPI;
            utils::Timer m_runTime;
            utils::Thread m_scriptThread;
            bool m_alreadyPeekedThisFrame;
    };

    using gClient = utils::Singleton<Client>;
};