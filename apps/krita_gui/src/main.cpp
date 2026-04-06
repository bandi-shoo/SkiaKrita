/// @file main.cpp
/// @brief SkiaKrita GUI – WinMain entry point.

#include "app.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

int WINAPI wWinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrev*/,
                    LPWSTR /*cmdLine*/, int /*nShow*/) {
    skiakrita::gui::App app;
    return app.run();
}
