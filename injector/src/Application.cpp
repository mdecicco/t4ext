#include <Application.h>

namespace t4ext {
    Application::Application() : utils::IWithLogging("Application") {
        m_game.subscribeLogger(this);

        
        HWND consoleWindow = GetConsoleWindow();

        // just for me
        // SetWindowPos(consoleWindow, 0, -1000, 200, 0, 0, SWP_NOSIZE | SWP_NOZORDER);	
        ShowWindow(consoleWindow, 0);
    }

    Application::~Application() {
    }

    bool Application::run() {
        if (!m_game.launch()) return false;
        if (!m_game.injectDLL()) return false;
        
        while (m_game.isRunning()) {
            Sleep(1000);
        }

        if (m_game.isRunning() && !m_game.terminate()) return false;

        return true;
    }

    void Application::onLogMessage(utils::LOG_LEVEL level, const utils::String& scope, const utils::String& message) {
        propagateLog(level, scope, message);

        utils::String msg = scope + ": " + message;
        printf("%s\n", msg.c_str());
    }
};