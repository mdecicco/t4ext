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
    // Application creation hook
    //
    Application* (Application::*CreateApplication_orig)(undefined4);
    Application* __fastcall CreateApplication_detour(Application* app, void* _EDX, undefined4 p1) {
        (app->*CreateApplication_orig)(p1);
        app->wantsFullScreen = false;
        return app;
    }

    //
    // CGame creation hook
    //
    CGame* (__cdecl *CreateEngine_orig)(undefined4);
    CGame* __cdecl CreateEngine_detour(undefined4 p1) {
        CGame* engine = CreateEngine_orig(p1);
        gClient::Get()->onEngineCreated(engine);
        return engine;
    }

    //
    // CLevel creation hook
    //
    CLevel* (CLevel::*ConstructLevel_orig)(const char* p1);
    CLevel* __fastcall ConstructLevel_detour(CLevel* level, void* _EDX, const char* p1) {
        (level->*ConstructLevel_orig)(p1);
        gClient::Get()->onLevelCreated(level);
        return level;
    }

    //
    // CLevel destruction hook
    //
    CLevel* (CLevel::*DestroyLevel_orig)(byte);
    CLevel* __fastcall DestroyLevel_detour(CLevel* level, void* _EDX, byte p1) {
        // not sure if this will ever be called or if the game exclusively uses CTurok4Level...
        // They are both the same size so it doesn't really matter which type we use, we just need
        // to track the lifetime of them
        gClient::Get()->onLevelDestroyed(level);
        (level->*DestroyLevel_orig)(p1);
        return level;
    }

    //
    // CTurok4Level destruction hook (it doesn't call the base dtor)
    //
    CLevel* (CLevel::*DestroyT4Level_orig)(byte);
    CLevel* __fastcall DestroyT4Level_detour(CLevel* level, void* _EDX, byte p1) {
        gClient::Get()->onLevelDestroyed(level);
        (level->*DestroyT4Level_orig)(p1);
        return level;
    }

    //
    // Actor creation hook
    //
    CActor* (CGame::*CreateActor_orig)(CLevel*, const char*, const char*, const char*);
    CActor* __fastcall CreateActor_detour(CGame* game, void * _EDX, CLevel* level, const char* name, const char* type, const char* mtfPath) {
        CActor* actor = (game->*CreateActor_orig)(level, name, type, mtfPath);
        gClient::Get()->onActorCreated(actor);
        return actor;
    }

    //
    // Actor destruction hook
    //
    void (CActor::*DestroyActor_orig)();
    void __fastcall DestroyActor_detour(CActor* actor, void * _EDX) {
        gClient::Get()->onActorDestroyed(actor);
        (actor->*DestroyActor_orig)();
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
        bool result = (app->*createWindow_orig)(title, 1920, 1080);
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
    // Game input processing
    //

    //
    // Hooks
    //
    void InstallHooks() {
        // Hook Application constructor
        MH_CreateHook((LPVOID)0x00401130, (LPVOID)&CreateApplication_detour, (LPVOID*)&CreateApplication_orig);
        MH_EnableHook((LPVOID)0x00401130);

        // redirect game's own debug logs
        MH_CreateHook((LPVOID)0x005c0760, (LPVOID)&FUN_005c0760_detour, (LPVOID*)&FUN_005c0760_orig);
        MH_EnableHook((LPVOID)0x005c0760);

        // Hook engine creation
        MH_CreateHook((LPVOID)0x00484b30, (LPVOID)&CreateEngine_detour, (LPVOID*)&CreateEngine_orig);
        MH_EnableHook((LPVOID)0x00484b30);

        // Hook level creation
        MH_CreateHook((LPVOID)0x004e0900, (LPVOID)&ConstructLevel_detour, (LPVOID*)&ConstructLevel_orig);
        MH_EnableHook((LPVOID)0x004e0900);

        // Hook level destruction
        MH_CreateHook((LPVOID)0x004e0ba0, (LPVOID)&DestroyT4Level_detour, (LPVOID*)&DestroyT4Level_orig);
        MH_EnableHook((LPVOID)0x004e0ba0);
        MH_CreateHook((LPVOID)0x00513750, (LPVOID)&DestroyLevel_detour, (LPVOID*)&DestroyLevel_orig);
        MH_EnableHook((LPVOID)0x00513750);

        // Hook actor creation
        MH_CreateHook((LPVOID)0x00481510, (LPVOID)&CreateActor_detour, (LPVOID*)&CreateActor_orig);
        MH_EnableHook((LPVOID)0x00481510);

        // Hook actor destruction
        MH_CreateHook((LPVOID)0x00522910, (LPVOID)&DestroyActor_detour, (LPVOID*)&DestroyActor_orig);
        MH_EnableHook((LPVOID)0x00522910);

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
        MH_RemoveHook((LPVOID)0x00522910);
        MH_RemoveHook((LPVOID)0x00481510);
        MH_RemoveHook((LPVOID)0x00513750);
        MH_RemoveHook((LPVOID)0x004e0ba0);
        MH_RemoveHook((LPVOID)0x004e0900);
        MH_RemoveHook((LPVOID)0x00484b30);
        MH_RemoveHook((LPVOID)0x005c0760);
        MH_RemoveHook((LPVOID)0x00401130);
    }
};