#pragma once
/// @file renderer.h
/// @brief Document compositing and brush-stroke rendering via Skia.
///
/// Public API is pure C++20 – Skia types are confined to the .cpp file.

#include <memory>
#include <string>

namespace skiakrita::core {
    class  Document;
    class  Layer;
    struct BrushStroke;
    struct Color;
} // namespace skiakrita::core

namespace skiakrita::graphics {

/// Composites a Document's layer stack and renders brush strokes.
class Renderer {
public:
    Renderer();
    ~Renderer();

    // Movable, non-copyable.
    Renderer(Renderer&&) noexcept;
    Renderer& operator=(Renderer&&) noexcept;
    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;

    /// Composite all visible layers of @p doc into an internal surface.
    bool compositeDocument(const core::Document& doc);

    /// Render a brush stroke into @p layer's pixel buffer using Skia.
    static void renderBrushStroke(core::Layer& layer,
                                  const core::BrushStroke& stroke);

    /// Fill a layer with a solid color.
    static void fillLayer(core::Layer& layer, const core::Color& color);

    /// Encode the most recent composite to a PNG file.
    bool saveComposite(const std::string& filePath);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::graphics
