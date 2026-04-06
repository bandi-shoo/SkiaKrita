#pragma once
/// @file layer.h
/// @brief Layer abstraction for SkiaKrita – raster pixel buffer with
///        visibility, opacity, and blend-mode properties.

#include <core/color.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace skiakrita::core {

/// Supported layer types.
enum class LayerType {
    Raster,
    Group,
    // Vector,  // future
};

/// A single layer in a Document.
/// Pixel data is stored as premultiplied RGBA8 (4 bytes per pixel).
class Layer {
public:
    Layer(std::string name, LayerType type, int width, int height);
    ~Layer();

    // Movable, non-copyable.
    Layer(Layer&&) noexcept;
    Layer& operator=(Layer&&) noexcept;
    Layer(const Layer&)            = delete;
    Layer& operator=(const Layer&) = delete;

    [[nodiscard]] const std::string& name() const;
    void setName(std::string name);

    [[nodiscard]] LayerType type() const;

    [[nodiscard]] bool visible() const;
    void setVisible(bool visible);

    [[nodiscard]] float opacity() const;
    void setOpacity(float opacity);  // clamped to [0, 1]

    [[nodiscard]] BlendMode blendMode() const;
    void setBlendMode(BlendMode mode);

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

    /// Raw pixel buffer (premultiplied RGBA8, row-major, top-to-bottom).
    [[nodiscard]] const std::vector<uint8_t>& pixels() const;
    [[nodiscard]] std::vector<uint8_t>& pixels();

    /// Fill entire layer with a solid color (premultiplied on write).
    void clear(Color color = Color::transparent());

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::core
