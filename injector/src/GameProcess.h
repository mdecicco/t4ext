#pragma once
#include <types.h>

#include <utils/ILogListener.h>

#include <windows.h>

namespace t4ext {
    class GameProcess : public utils::IWithLogging {
        public:
            GameProcess();
            virtual ~GameProcess();

            bool launch();
            bool terminate();
            bool isRunning();
            bool injectDLL();
        
        protected:
            STARTUPINFO m_startupInfo;
            PROCESS_INFORMATION m_procInfo;
    };
};