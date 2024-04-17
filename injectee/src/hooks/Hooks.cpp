#include <hooks/Hooks.h>
#include <hooks/d3d.h>

#include <core/CGame.h>
#include <core/CLevel.h>
#include <core/CActor.h>
#include <core/Application.h>
#include <Client.h>

#include <utils/Singleton.hpp>
#include <MinHook.h>

namespace t4ext {
    constexpr bool mute_game_logs = true;

    //
    // Debug logging hook
    //
    undefined4 (*FUN_005c0760_orig)(const char* fmt, ...) = nullptr;
    undefined4 FUN_005c0760_detour(const char* fmt, ...) {
        va_list ap;
        va_start(ap, fmt);
        i32 count = 0;
        
        if constexpr (mute_game_logs) {
            // not sure if anyone actually uses the output of printf but just in case
            char buf[2048] = { 0 };
            count = vsnprintf(buf, 2048, fmt, ap);
        } else {
            printf("[Game] ");
            count = vprintf(fmt, ap);
        }
        
        va_end(ap);

        return count;
    }

    //
    // Actor creation hook
    //
    CActor* (CGame::*CreateActor_orig)(CLevel*, const char*, const char*, const char*);
    CActor* __fastcall CreateActor_detour(CGame* game, void * _EDX, CLevel* level, const char* name, const char* type, const char* mtfPath) {
        CActor* actor = (game->*CreateActor_orig)(level, name, type, mtfPath);
        gClient::Get()->onActorCreated(level, actor, name, type, mtfPath);
        return actor;
    }

    //
    // Engine update
    //
    i32 (Application::*AppUpdate_orig)();
    i32 __fastcall AppUpdate_detour(Application* app, void* _EDX) {
        gClient::Get()->onBeforeUpdate();
        i32 result = (app->*AppUpdate_orig)();
        return result;
    }

    //
    // Window creation
    //
    bool (Application::*createWindow_orig)(const char*, u32, u32);
    bool __fastcall createWindow_detour(Application* app, void* _EDX, const char* title, u32 width, u32 height) {
        app->windowStyle = WS_OVERLAPPEDWINDOW;
        bool result = (app->*createWindow_orig)(title, 1280, 720);
        if (result) gClient::Get()->onWindowCreated(app->windowHandle);
        return result;
    }

    //
    // PeekMessageA
    //
    BOOL (WINAPI *PeekMessageA_orig)(LPMSG, HWND, UINT, UINT, UINT);
    BOOL WINAPI PeekMessageA_detour(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
        BOOL result = PeekMessageA_orig(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);
        gClient::Get()->onPeekMessage(lpMsg, hWnd);
        return result;
    }

    //
    // Hooks
    //
    void InstallHooks() {
        // redirect game's own debug logs
        MH_CreateHook((LPVOID)0x005c0760, (LPVOID)&FUN_005c0760_detour, (LPVOID*)&FUN_005c0760_orig);
        MH_EnableHook((LPVOID)0x005c0760);

        // Hook actor creation to get notifications about it
        MH_CreateHook((LPVOID)0x00481510, (LPVOID)&CreateActor_detour, (LPVOID*)&CreateActor_orig);
        MH_EnableHook((LPVOID)0x00481510);

        // Hook game updates
        MH_CreateHook((LPVOID)0x005e0360, (LPVOID)&AppUpdate_detour, (LPVOID*)&AppUpdate_orig);
        MH_EnableHook((LPVOID)0x005e0360);

        // Hook window create
        MH_CreateHook((LPVOID)0x005e0f20, (LPVOID)&createWindow_detour, (LPVOID*)&createWindow_orig);
        MH_EnableHook((LPVOID)0x005e0f20);

        // Hook PeekMessageA
        MH_CreateHook((LPVOID)*(void**)0x0062a224, (LPVOID)&PeekMessageA_detour, (LPVOID*)&PeekMessageA_orig);
        MH_EnableHook((LPVOID)*(void**)0x0062a224);

        InstallD3DHooks();
    }

    void UninstallHooks() {
        UninstallD3DHooks();
        
        MH_RemoveHook((LPVOID)*(void**)0x0062a224);
        MH_RemoveHook((LPVOID)0x005e0f20);
        MH_RemoveHook((LPVOID)0x005e0360);
        MH_RemoveHook((LPVOID)0x00481510);
        MH_RemoveHook((LPVOID)0x005c0760);
    }
};