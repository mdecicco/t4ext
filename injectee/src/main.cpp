#include <Client.h>
#include <utils/Allocator.hpp>
#include <utils/Singleton.hpp>

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            utils::Mem::Create();
            utils::Singleton<t4ext::Client>::Create();
            t4ext::Client* c = utils::Singleton<t4ext::Client>::Get();
            if (!c->init()) return FALSE;
            break;
        }
        case DLL_PROCESS_DETACH: {
            utils::Singleton<t4ext::Client>::Destroy();
            utils::Mem::Destroy();
            break;
        }
	}

	return TRUE;
}