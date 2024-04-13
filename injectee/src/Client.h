#pragma once
#include <types.h>
#include <utils/ILogListener.h>
#include <utils/Thread.h>
#include <utils/Timer.h>

#include <stdio.h>

namespace t4ext {
    class CActor;
    class CLevel;
    class TypeScriptAPI;

    class Client : public utils::IWithLogging {
        public:
            Client();
            virtual ~Client();

            bool init();
            void run();
            f32 elapsedTime();
            CLevel* currentLevel();

            void onActorCreated(CLevel* lvl, CActor* actor, const char* name, const char* type, const char* model);

            virtual void onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message);
    
        protected:
            FILE* m_logFile;
            utils::Thread m_clientThread;

            CLevel* m_currentLevel;
            TypeScriptAPI* m_scriptAPI;
            utils::Timer m_runTime;
    };

    using gClient = utils::Singleton<Client>;
};