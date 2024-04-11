#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

#include <windows.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            // just for me
            AllocConsole();
            HWND consoleWindow = FindWindowA("ConsoleWindowClass", NULL);
            SetWindowPos(consoleWindow, 0, -1200, 300, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

            utils::Mem::Create();
            utils::Singleton<t4ext::Client>::Create();
            t4ext::Client* c = utils::Singleton<t4ext::Client>::Get();
            if (!c->init()) return FALSE;
            break;
        }
        case DLL_PROCESS_DETACH: {
            utils::Singleton<t4ext::Client>::Destroy();
            utils::Mem::Destroy();
            exit(0);
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH: break;
	}

	return TRUE;
}