#pragma once
#include <types.h>
#include <GameProcess.h>

namespace t4ext {
    class Application : public utils::IWithLogging {
        public:
            Application();
            virtual ~Application();

            bool run();

            virtual void onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message);
    
        protected:
            GameProcess m_game;
    };
};