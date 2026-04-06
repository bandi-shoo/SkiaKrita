/// @file renderer.cpp
/// @brief Renderer implementation -- all Skia headers confined here.

#include "graphics_skia/renderer.h"

#include <core/brush_engine.h>
#include <core/color.h>
#include <core/document.h>
#include <core/layer.h>

// Skia headers (never leaked through the public interface)
#include <core/SkBitmap.h>
#include <core/SkCanvas.h>
#include <core/SkColor.h>
#include <core/SkImage.h>
#include <core/SkImageInfo.h>
#include <core/SkPaint.h>
#include <core/SkPath.h>
#include <core/SkPixmap.h>
#include <core/SkStream.h>
#include <core/SkSurface.h>
#include <encode/SkPngEncoder.h>

namespace skiakrita::graphics {

// -- Helpers (anonymous namespace) ----------------------------------------
namespace {

SkBlendMode toSkBlendMode(core::BlendMode mode) {
    switch (mode) {
        case core::BlendMode::Normal:    return SkBlendMode::kSrcOver;
        case core::BlendMode::Multiply:  return SkBlendMode::kMultiply;
        case core::BlendMode::Screen:    return SkBlendMode::kScreen;
        case core::BlendMode::Overlay:   return SkBlendMode::kOverlay;
        case core::BlendMode::Darken:    return SkBlendMode::kDarken;
        case core::BlendMode::Lighten:   return SkBlendMode::kLighten;
        case core::BlendMode::SoftLight: return SkBlendMode::kSoftLight;
        case core::BlendMode::HardLight: return SkBlendMode::kHardLight;
    }
    return SkBlendMode::kSrcOver;
}

} // anonymous namespace

// -- Pimpl ----------------------------------------------------------------
struct Renderer::Impl {
    sk_sp<SkSurface> compositeSurface;
    int width  = 0;
    int height = 0;
};

// -- Lifetime -------------------------------------------------------------
Renderer::Renderer()  : m_impl(std::make_unique<Impl>()) {}
Renderer::~Renderer()                                    = default;
Renderer::Renderer(Renderer&&) noexcept                  = default;
Renderer& Renderer::operator=(Renderer&&) noexcept       = default;

// -- Compositing ----------------------------------------------------------
bool Renderer::compositeDocument(const core::Document& doc) {
    const int w = doc.width();
    const int h = doc.height();
    if (w <= 0 || h <= 0) return false;

    const auto info = SkImageInfo::MakeN32Premul(w, h);
    m_impl->compositeSurface = SkSurfaces::Raster(info);
    if (!m_impl->compositeSurface) return false;

    m_impl->width  = w;
    m_impl->height = h;

    SkCanvas* canvas = m_impl->compositeSurface->getCanvas();
    canvas->clear(SK_ColorWHITE);

    // Composite layers bottom-to-top
    for (std::size_t i = 0; i < doc.layerCount(); ++i) {
        const auto& layer = doc.layerAt(i);
        if (!layer.visible() || layer.opacity() <= 0.f) continue;

        const auto& px = layer.pixels();
        const auto layerInfo = SkImageInfo::Make(
            layer.width(), layer.height(),
            kRGBA_8888_SkColorType, kPremul_SkAlphaType);

        SkBitmap bmp;
        bmp.installPixels(layerInfo,
                          const_cast<uint8_t*>(px.data()),
                          static_cast<size_t>(layer.width()) * 4);
        sk_sp<SkImage> image = bmp.asImage();
        if (!image) continue;

        SkPaint paint;
        paint.setAlphaf(layer.opacity());
        paint.setBlendMode(toSkBlendMode(layer.blendMode()));
        canvas->drawImage(image, 0, 0, SkSamplingOptions{}, &paint);
    }

    return true;
}

// -- Composite buffer access ----------------------------------------------
CompositeBuffer Renderer::getCompositeBuffer() const {
    CompositeBuffer buf;
    if (!m_impl->compositeSurface) return buf;

    SkPixmap pixmap;
    if (m_impl->compositeSurface->peekPixels(&pixmap)) {
        buf.data   = pixmap.addr();
        buf.width  = pixmap.width();
        buf.height = pixmap.height();
        buf.stride = pixmap.rowBytes();
    }
    return buf;
}

// -- Brush stroke rendering (pressure-sensitive) --------------------------
void Renderer::renderBrushStroke(core::Layer& layer,
                                 const core::BrushStroke& stroke) {
    if (stroke.empty()) return;

    const int w = layer.width();
    const int h = layer.height();
    auto& px    = layer.pixels();

    const auto info = SkImageInfo::Make(
        w, h, kRGBA_8888_SkColorType, kPremul_SkAlphaType);
    auto surface = SkSurfaces::WrapPixels(
        info, px.data(), static_cast<size_t>(w) * 4);
    if (!surface) return;

    SkCanvas* canvas = surface->getCanvas();

    const auto& s = stroke.settings;
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(SkColorSetARGB(
        static_cast<uint8_t>(s.opacity * 255.f),
        s.color.r, s.color.g, s.color.b));
    paint.setStrokeCap(SkPaint::kRound_Cap);
    paint.setStyle(SkPaint::kStroke_Style);

    if (stroke.points.size() == 1) {
        // Single dab
        paint.setStyle(SkPaint::kFill_Style);
        canvas->drawCircle(
            stroke.points[0].x, stroke.points[0].y,
            s.size * 0.5f * stroke.points[0].pressure, paint);
    } else {
        // Per-segment rendering with pressure-sensitive width
        for (std::size_t i = 1; i < stroke.points.size(); ++i) {
            const auto& p0 = stroke.points[i - 1];
            const auto& p1 = stroke.points[i];
            const float avgPressure = (p0.pressure + p1.pressure) * 0.5f;
            paint.setStrokeWidth(s.size * avgPressure);
            canvas->drawLine(p0.x, p0.y, p1.x, p1.y, paint);
        }
    }
}

// -- Fill -----------------------------------------------------------------
void Renderer::fillLayer(core::Layer& layer, const core::Color& color) {
    layer.clear(color);
}

// -- Export ----------------------------------------------------------------
bool Renderer::saveComposite(const std::string& filePath) {
    if (!m_impl->compositeSurface) return false;

    sk_sp<SkImage> image = m_impl->compositeSurface->makeImageSnapshot();
    if (!image) return false;

    SkPixmap pixmap;
    if (!image->peekPixels(&pixmap)) return false;

    SkFILEWStream stream(filePath.c_str());
    if (!stream.isValid()) return false;

    return SkPngEncoder::Encode(&stream, pixmap, {});
}

} // namespace skiakrita::graphics
