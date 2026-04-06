#pragma once
/// @file document.h
/// @brief Document model – the top-level container for layers and canvas.

#include <core/layer.h>

#include <cstddef>
#include <memory>
#include <string>

namespace skiakrita::core {

/// A Document owns an ordered stack of Layers and defines canvas dimensions.
class Document {
public:
    Document(std::string name, int width, int height);
    ~Document();

    // Movable, non-copyable.
    Document(Document&&) noexcept;
    Document& operator=(Document&&) noexcept;
    Document(const Document&)            = delete;
    Document& operator=(const Document&) = delete;

    [[nodiscard]] const std::string& name() const;
    void setName(std::string name);

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

    // ── Layer management ─────────────────────────────────────────
    Layer& addLayer(std::string layerName,
                    LayerType type = LayerType::Raster);

    [[nodiscard]] std::size_t layerCount() const;
    [[nodiscard]] Layer&       layerAt(std::size_t index);
    [[nodiscard]] const Layer& layerAt(std::size_t index) const;

    void removeLayer(std::size_t index);
    void moveLayer(std::size_t from, std::size_t to);

    // ── Active layer ─────────────────────────────────────────────
    [[nodiscard]] std::size_t activeLayerIndex() const;
    void setActiveLayerIndex(std::size_t index);
    [[nodiscard]] Layer& activeLayer();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::core
