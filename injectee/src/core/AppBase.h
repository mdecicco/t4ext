#pragma once
#include <types.h>

#define NOMINMAX
#include <windows.h>

namespace t4ext {
    class AppBase {
        public:
            virtual ~AppBase();

            virtual bool createWindow(const char* className, const char* title, u32 width, u32 height);
            virtual void createDialog(DWORD dialogTemplateResourceId, const char* className);
            virtual void FUN_005e6fe6();
            virtual bool loadCursors();
            virtual bool setClassName(const char* className, bool p2);
            virtual bool createDialog(DWORD dialogTemplateResourceId);
            virtual bool createWindow(const char* title, u32 width, u32 height);
            virtual void FUN_005e1070(LPRECT p1);

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