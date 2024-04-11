#include <GameProcess.h>

namespace t4ext {
    GameProcess::GameProcess() : utils::IWithLogging("GameProcess") {
        ZeroMemory(&m_startupInfo, sizeof(STARTUPINFO));
        m_startupInfo.cb = sizeof(STARTUPINFO);

        ZeroMemory(&m_procInfo, sizeof(PROCESS_INFORMATION));
    }

    GameProcess::~GameProcess() {
        terminate();
    }

    bool GameProcess::launch() {
        if (m_procInfo.hProcess) return false;

        log("Attempting to launch Turok4.exe...");

        BOOL result = CreateProcess(
            "C:\\Program Files (x86)\\Acclaim Entertainment\\Turok Evolution\\Turok4.exe",
            nullptr,
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &m_startupInfo,
            &m_procInfo
        );

        if (!result) error("Failed to launch");

        return result != 0;
    }

    bool GameProcess::terminate() {
        if (!m_procInfo.hProcess) return false;

        log("Attempting to terminate Turok4.exe...");

        BOOL result = TerminateProcess(m_procInfo.hProcess, 0);

        if (!result) {
            error("Failed to terminate");
            return false;
        }

        CloseHandle(m_procInfo.hProcess);
        CloseHandle(m_procInfo.hThread);

        ZeroMemory(&m_startupInfo, sizeof(STARTUPINFO));
        m_startupInfo.cb = sizeof(STARTUPINFO);

        ZeroMemory(&m_procInfo, sizeof(PROCESS_INFORMATION));

        return true;
    }

    bool GameProcess::isRunning() {
        DWORD code;
        if (GetExitCodeProcess(m_procInfo.hProcess, &code) == 0) {
            error("Call to GetExitCodeProcess failed");
            return false;
        }

        if (code == STILL_ACTIVE) return true;

        CloseHandle(m_procInfo.hProcess);
        CloseHandle(m_procInfo.hThread);

        ZeroMemory(&m_startupInfo, sizeof(STARTUPINFO));
        m_startupInfo.cb = sizeof(STARTUPINFO);

        ZeroMemory(&m_procInfo, sizeof(PROCESS_INFORMATION));

        return false;
    }

    bool GameProcess::injectDLL() {
        if (!m_procInfo.hProcess) return false;

        log("Attempting to inject DLL...");

        HMODULE hKernel32 = GetModuleHandleA("Kernel32");
        if (hKernel32 == NULL) {
            error("Failed to obtain handle to kernel module");
            return false;
        }

        FARPROC pLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");
        if (!pLoadLibraryA) {
            error("Failed to get address of LoadLibraryA");
            return false;
        }

        const char* dllPath = "C:\\Users\\miguel\\programming\\t4ext\\bin\\t4ext.dll\0";
        size_t dllPathSz = strlen(dllPath);

        LPVOID dllPathMem = VirtualAllocEx(m_procInfo.hProcess, nullptr, dllPathSz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (dllPathMem == nullptr) {
            error("Failed to allocate remote memory for DLL path");
            return false;
        }

        if (!WriteProcessMemory(m_procInfo.hProcess, dllPathMem, (LPCVOID)dllPath, dllPathSz, nullptr)) {
            error("Failed to write DLL path to remote memory");

            if (VirtualFreeEx(m_procInfo.hProcess, dllPathMem, 0, MEM_RELEASE) == 0) {
                error("Also, failed to free remote memory for DLL path...");
            }

            return false;
        }
        
        HANDLE thread = CreateRemoteThread(
            m_procInfo.hProcess,
            nullptr,
            0,
            (LPTHREAD_START_ROUTINE)pLoadLibraryA,
            dllPathMem,
            0,
            nullptr
        );

        if (!thread) {
            error("Failed to spawn remote thread");

            if (VirtualFreeEx(m_procInfo.hProcess, dllPathMem, 0, MEM_RELEASE) == 0) {
                error("Also, failed to free remote memory for DLL path...");
            }

            return false;
        }

        WaitForSingleObject(thread, INFINITE);
        DWORD result = 0;
        GetExitCodeThread(thread, &result);
        CloseHandle(thread);

        if (result == 0) {
            error("Failed to load DLL in remote process");
            return false;
        }

        log("Successfully injected DLL");
        return true;
    }
};