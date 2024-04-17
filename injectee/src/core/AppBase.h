#pragma once
#include <types.h>

#define NOMINMAX
#include <windows.h>

namespace t4ext {
    class AppBase {
        public:
            AppBase();
            virtual ~AppBase() = 0;

            virtual bool createWindow(const char* className, const char* title, u32 width, u32 height) = 0;
            virtual void createDialog(DWORD dialogTemplateResourceId, const char* className) = 0;
            virtual void FUN_005e6fe6() = 0;
            virtual bool loadCursors() = 0;
            virtual bool setClassName(const char* className, bool p2) = 0;
            virtual bool createDialog(DWORD dialogTemplateResourceId) = 0;
            virtual bool createWindow(const char* title, u32 width, u32 height) = 0;
            virtual void FUN_005e1070(LPRECT p1) = 0;

            static AppBase* Get();

            undefined4 field_0x4;
            HINSTANCE instance;
            HWND windowHandle;
            DWORD dialogTemplateResourceId;
            DWORD windowStyle;
            char windowTitle[256];
            char className[256];
            u32 windowWidth;
            u32 windowHeight;
            tagRECT windowRectA;
            tagRECT windowRectB;
            HMENU hMenu;
            HICON appIcon;
            HACCEL accelTable;
            HCURSOR cursorIcon0;
            HCURSOR cursorIcon1;
            HCURSOR cursorIcon2;
    };
};