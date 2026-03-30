/// @file skia_context.cpp
/// @brief SkiaContext implementation – all Skia headers confined here.

#include "graphics_skia/skia_context.h"

// Skia headers (never leaked through the public interface)
#include <core/SkCanvas.h>
#include <core/SkColor.h>
#include <core/SkData.h>
#include <core/SkImage.h>
#include <core/SkImageInfo.h>
#include <core/SkPaint.h>
#include <core/SkRect.h>
#include <core/SkStream.h>
#include <core/SkSurface.h>
#include <encode/SkPngEncoder.h>

#include <format>

namespace skiakrita::graphics {

// ── Pimpl ────────────────────────────────────────────────────────
struct SkiaContext::Impl {
    sk_sp<SkSurface> surface;
    int width  = 0;
    int height = 0;
};

// ── Lifetime ─────────────────────────────────────────────────────
SkiaContext::SkiaContext()                            : m_impl(std::make_unique<Impl>()) {}
SkiaContext::~SkiaContext()                           = default;
SkiaContext::SkiaContext(SkiaContext&&) noexcept       = default;
SkiaContext& SkiaContext::operator=(SkiaContext&&) noexcept = default;

// ── Static info ──────────────────────────────────────────────────
std::string SkiaContext::getBackendInfo() {
    return "Skia raster backend (CPU)";
}

// ── Surface management ───────────────────────────────────────────
bool SkiaContext::createSurface(int width, int height) {
    if (width <= 0 || height <= 0) return false;

    const auto info = SkImageInfo::MakeN32Premul(width, height);
    m_impl->surface = SkSurfaces::Raster(info);
    if (!m_impl->surface) return false;

    m_impl->width  = width;
    m_impl->height = height;
    return true;
}

SurfaceInfo SkiaContext::getSurfaceInfo() const {
    return { m_impl->width, m_impl->height, m_impl->surface != nullptr };
}

// ── Drawing ──────────────────────────────────────────────────────
bool SkiaContext::drawTestPattern() {
    if (!m_impl->surface) return false;

    SkCanvas* canvas = m_impl->surface->getCanvas();
    canvas->clear(SK_ColorWHITE);

    SkPaint paint;
    paint.setAntiAlias(true);

    // Red rectangle
    paint.setColor(SK_ColorRED);
    paint.setStyle(SkPaint::kFill_Style);
    canvas->drawRect(SkRect::MakeXYWH(10, 10, 100, 80), paint);

    // Blue circle
    paint.setColor(SK_ColorBLUE);
    canvas->drawCircle(180.f, 128.f, 50.f, paint);

    // Green line
    paint.setColor(SK_ColorGREEN);
    paint.setStrokeWidth(4.f);
    paint.setStyle(SkPaint::kStroke_Style);
    canvas->drawLine(10.f, 200.f, 240.f, 200.f, paint);

    return true;
}

// ── Export ────────────────────────────────────────────────────────
bool SkiaContext::saveSnapshot(const std::string& filePath) {
    if (!m_impl->surface) return false;

    sk_sp<SkImage> image = m_impl->surface->makeImageSnapshot();
    if (!image) return false;

    SkPixmap pixmap;
    if (!image->peekPixels(&pixmap)) return false;

    SkFILEWStream stream(filePath.c_str());
    if (!stream.isValid()) return false;

    return SkPngEncoder::Encode(&stream, pixmap, {});
}

} // namespace skiakrita::graphics
