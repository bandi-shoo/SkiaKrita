#pragma once
/// @file win32_window.h
/// @brief Minimal Win32 window wrapper with paint / mouse / key / resize callbacks.

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <core/input_event.h>

#include <functional>
#include <memory>

namespace skiakrita::gui {

/// Thin RAII wrapper around a Win32 HWND.
class Win32Window {
public:
    Win32Window(int clientWidth, int clientHeight, const wchar_t* title);
    ~Win32Window();

    Win32Window(const Win32Window&)            = delete;
    Win32Window& operator=(const Win32Window&) = delete;

    [[nodiscard]] HWND handle() const;
    [[nodiscard]] int  clientWidth() const;
    [[nodiscard]] int  clientHeight() const;

    void show();
    void invalidate();

    // -- Callbacks --------------------------------------------------------
    using PaintCallback  = std::function<void(HDC hdc, int w, int h)>;
    using MouseCallback  = std::function<void(const core::MouseEvent&)>;
    using KeyCallback    = std::function<void(const core::KeyEvent&)>;
    using ResizeCallback = std::function<void(int w, int h)>;

    void setPaintCallback (PaintCallback  cb);
    void setMouseCallback (MouseCallback  cb);
    void setKeyCallback   (KeyCallback    cb);
    void setResizeCallback(ResizeCallback cb);

    /// Enter the Win32 message loop (blocks until WM_QUIT).
    static int runMessageLoop();

private:
    static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
    static void ensureClassRegistered();

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::gui
