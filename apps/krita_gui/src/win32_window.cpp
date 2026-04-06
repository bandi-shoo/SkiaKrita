/// @file win32_window.cpp
/// @brief Win32 window implementation.

#include "win32_window.h"

#include <windowsx.h>   // GET_X_LPARAM, GET_Y_LPARAM

namespace skiakrita::gui {

// ── Pimpl ────────────────────────────────────────────────────────
struct Win32Window::Impl {
    HWND            hwnd          = nullptr;
    int             clientW       = 0;
    int             clientH       = 0;
    bool            lButtonDown   = false;
    PaintCallback   paintCb;
    MouseCallback   mouseCb;
    ResizeCallback  resizeCb;
};

// ── Class registration (once) ────────────────────────────────────
static constexpr const wchar_t* kClassName = L"SkiaKritaWindow";
static bool sClassRegistered = false;

void Win32Window::ensureClassRegistered() {
    if (sClassRegistered) return;

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = Win32Window::wndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.hCursor       = LoadCursorW(nullptr, IDC_CROSS);
    wc.hbrBackground = nullptr;   // we paint everything ourselves
    wc.lpszClassName = kClassName;
    RegisterClassExW(&wc);

    sClassRegistered = true;
}

// ── Lifetime ─────────────────────────────────────────────────────
Win32Window::Win32Window(int clientWidth, int clientHeight,
                         const wchar_t* title)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->clientW = clientWidth;
    m_impl->clientH = clientHeight;

    ensureClassRegistered();

    RECT rc = {0, 0, static_cast<LONG>(clientWidth),
                     static_cast<LONG>(clientHeight)};
    AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, 0);

    m_impl->hwnd = CreateWindowExW(
        0, kClassName, title, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, GetModuleHandleW(nullptr),
        this);   // pass 'this' for WM_NCCREATE
}

Win32Window::~Win32Window() {
    if (m_impl->hwnd) DestroyWindow(m_impl->hwnd);
}

HWND Win32Window::handle()       const { return m_impl->hwnd; }
int  Win32Window::clientWidth()  const { return m_impl->clientW; }
int  Win32Window::clientHeight() const { return m_impl->clientH; }

void Win32Window::show() {
    ShowWindow(m_impl->hwnd, SW_SHOW);
    UpdateWindow(m_impl->hwnd);
}

void Win32Window::invalidate() {
    InvalidateRect(m_impl->hwnd, nullptr, FALSE);
}

void Win32Window::setPaintCallback (PaintCallback  cb) { m_impl->paintCb  = std::move(cb); }
void Win32Window::setMouseCallback (MouseCallback  cb) { m_impl->mouseCb  = std::move(cb); }
void Win32Window::setResizeCallback(ResizeCallback cb) { m_impl->resizeCb = std::move(cb); }

// ── Message loop ─────────────────────────────────────────────────
int Win32Window::runMessageLoop() {
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return static_cast<int>(msg.wParam);
}

// ── Window procedure ─────────────────────────────────────────────
LRESULT CALLBACK Win32Window::wndProc(HWND hwnd, UINT msg,
                                      WPARAM wp, LPARAM lp) {
    Win32Window* self = nullptr;

    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        self = static_cast<Win32Window*>(cs->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(self));
    } else {
        self = reinterpret_cast<Win32Window*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (!self) return DefWindowProcW(hwnd, msg, wp, lp);

    switch (msg) {

    // ── Paint ────────────────────────────────────────────────────
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (self->m_impl->paintCb)
            self->m_impl->paintCb(hdc, self->m_impl->clientW,
                                       self->m_impl->clientH);
        EndPaint(hwnd, &ps);
        return 0;
    }

    // ── Mouse ────────────────────────────────────────────────────
    case WM_LBUTTONDOWN: {
        SetCapture(hwnd);
        self->m_impl->lButtonDown = true;
        if (self->m_impl->mouseCb) {
            core::MouseEvent e;
            e.type     = core::MouseEvent::Type::Press;
            e.x        = static_cast<float>(GET_X_LPARAM(lp));
            e.y        = static_cast<float>(GET_Y_LPARAM(lp));
            e.pressure = 1.f;
            e.button   = core::MouseButton::Left;
            self->m_impl->mouseCb(e);
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        if (self->m_impl->lButtonDown && self->m_impl->mouseCb) {
            core::MouseEvent e;
            e.type     = core::MouseEvent::Type::Move;
            e.x        = static_cast<float>(GET_X_LPARAM(lp));
            e.y        = static_cast<float>(GET_Y_LPARAM(lp));
            e.pressure = 1.f;
            e.button   = core::MouseButton::Left;
            self->m_impl->mouseCb(e);
        }
        return 0;
    }
    case WM_LBUTTONUP: {
        ReleaseCapture();
        self->m_impl->lButtonDown = false;
        if (self->m_impl->mouseCb) {
            core::MouseEvent e;
            e.type     = core::MouseEvent::Type::Release;
            e.x        = static_cast<float>(GET_X_LPARAM(lp));
            e.y        = static_cast<float>(GET_Y_LPARAM(lp));
            e.pressure = 1.f;
            e.button   = core::MouseButton::Left;
            self->m_impl->mouseCb(e);
        }
        return 0;
    }

    // ── Resize ───────────────────────────────────────────────────
    case WM_SIZE: {
        self->m_impl->clientW = LOWORD(lp);
        self->m_impl->clientH = HIWORD(lp);
        if (self->m_impl->resizeCb)
            self->m_impl->resizeCb(self->m_impl->clientW,
                                   self->m_impl->clientH);
        return 0;
    }

    // ── Close ────────────────────────────────────────────────────
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

} // namespace skiakrita::gui
