/// @file app.cpp
/// @brief Application controller implementation.

#include "app.h"
#include "win32_window.h"

#include <core/document.h>
#include <core/brush_tool.h>
#include <graphics_skia/renderer.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

namespace skiakrita::gui {

// ── Constants ────────────────────────────────────────────────────
static constexpr int kCanvasW = 800;
static constexpr int kCanvasH = 600;

// ── Pimpl ────────────────────────────────────────────────────────
struct App::Impl {
    core::Document              doc{"Untitled", kCanvasW, kCanvasH};
    core::BrushTool             brushTool;
    graphics::Renderer          renderer;
    std::unique_ptr<Win32Window> window;

    void onPaint(HDC hdc, int winW, int winH);
    void onMouse(const core::MouseEvent& e);
};

// ── Paint handler ────────────────────────────────────────────────
void App::Impl::onPaint(HDC hdc, int winW, int winH) {
    renderer.compositeDocument(doc);
    const auto buf = renderer.getCompositeBuffer();
    if (!buf.data) return;

    // N32 on Windows = BGRA – matches BITMAPINFO with BI_RGB / 32bpp
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = buf.width;
    bmi.bmiHeader.biHeight      = -buf.height;   // top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(hdc,
                  0, 0, winW, winH,            // destination
                  0, 0, buf.width, buf.height,  // source
                  buf.data, &bmi,
                  DIB_RGB_COLORS, SRCCOPY);
}

// ── Mouse handler ────────────────────────────────────────────────
void App::Impl::onMouse(const core::MouseEvent& e) {
    brushTool.handleEvent(doc, e);
    window->invalidate();
}

// ── App lifetime ─────────────────────────────────────────────────
App::App() : m_impl(std::make_unique<Impl>()) {
    // Document layers
    m_impl->doc.addLayer("Background").clear(core::Color::white());
    m_impl->doc.addLayer("Paint");
    m_impl->doc.setActiveLayerIndex(1);

    // Brush configuration
    m_impl->brushTool.settings().color = core::Color::black();
    m_impl->brushTool.settings().size  = 4.f;
    m_impl->brushTool.setRenderCallback(
        [](core::Layer& layer, const core::BrushStroke& stroke) {
            graphics::Renderer::renderBrushStroke(layer, stroke);
        });

    // Window + callbacks
    m_impl->window = std::make_unique<Win32Window>(
        kCanvasW, kCanvasH, L"SkiaKrita");

    m_impl->window->setPaintCallback(
        [this](HDC hdc, int w, int h) { m_impl->onPaint(hdc, w, h); });

    m_impl->window->setMouseCallback(
        [this](const core::MouseEvent& e) { m_impl->onMouse(e); });
}

App::~App() = default;

int App::run() {
    m_impl->window->show();
    return Win32Window::runMessageLoop();
}

} // namespace skiakrita::gui
