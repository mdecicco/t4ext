#pragma once
#include <types.h>
#include <utils/ILogListener.h>
#include <utils/Thread.h>

#include <stdio.h>

namespace t4ext {
    class CActor;
    class CLevel;

    class Client : public utils::IWithLogging {
        public:
            Client();
            virtual ~Client();

            bool init();
            void run();

            void onActorCreated(CLevel* lvl, CActor* actor, const char* name, const char* type, const char* model);

            virtual void onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message);
    
        protected:
            FILE* m_logFile;
            utils::Thread m_clientThread;

            CLevel* m_currentLevel;
    };
};