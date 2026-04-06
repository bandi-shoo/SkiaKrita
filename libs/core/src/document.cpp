/// @file document.cpp
/// @brief Document model implementation.

#include "core/document.h"

#include <stdexcept>
#include <vector>

namespace skiakrita::core {

// ── Pimpl ────────────────────────────────────────────────────────
struct Document::Impl {
    std::string                          name;
    int                                  width;
    int                                  height;
    std::vector<std::unique_ptr<Layer>>  layers;
    std::size_t                          activeIndex = 0;
};

// ── Lifetime ─────────────────────────────────────────────────────
Document::Document(std::string name, int width, int height)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->name   = std::move(name);
    m_impl->width  = width;
    m_impl->height = height;
}

Document::~Document()                                = default;
Document::Document(Document&&) noexcept              = default;
Document& Document::operator=(Document&&) noexcept   = default;

// ── Accessors ────────────────────────────────────────────────────
const std::string& Document::name()  const { return m_impl->name; }
void Document::setName(std::string n)      { m_impl->name = std::move(n); }

int Document::width()  const { return m_impl->width; }
int Document::height() const { return m_impl->height; }

// ── Layer management ─────────────────────────────────────────────
Layer& Document::addLayer(std::string layerName, LayerType type) {
    auto layer = std::make_unique<Layer>(
        std::move(layerName), type, m_impl->width, m_impl->height);
    m_impl->layers.push_back(std::move(layer));
    return *m_impl->layers.back();
}

std::size_t Document::layerCount() const {
    return m_impl->layers.size();
}

Layer& Document::layerAt(std::size_t index) {
    if (index >= m_impl->layers.size())
        throw std::out_of_range("Layer index out of range");
    return *m_impl->layers[index];
}

const Layer& Document::layerAt(std::size_t index) const {
    if (index >= m_impl->layers.size())
        throw std::out_of_range("Layer index out of range");
    return *m_impl->layers[index];
}

void Document::removeLayer(std::size_t index) {
    if (index >= m_impl->layers.size())
        throw std::out_of_range("Layer index out of range");
    m_impl->layers.erase(
        m_impl->layers.begin() + static_cast<std::ptrdiff_t>(index));
    if (m_impl->activeIndex >= m_impl->layers.size() &&
        !m_impl->layers.empty()) {
        m_impl->activeIndex = m_impl->layers.size() - 1;
    }
}

void Document::moveLayer(std::size_t from, std::size_t to) {
    if (from >= m_impl->layers.size() || to >= m_impl->layers.size())
        throw std::out_of_range("Layer index out of range");
    if (from == to) return;
    auto layer = std::move(m_impl->layers[from]);
    m_impl->layers.erase(
        m_impl->layers.begin() + static_cast<std::ptrdiff_t>(from));
    m_impl->layers.insert(
        m_impl->layers.begin() + static_cast<std::ptrdiff_t>(to),
        std::move(layer));
}

// ── Active layer ─────────────────────────────────────────────────
std::size_t Document::activeLayerIndex() const {
    return m_impl->activeIndex;
}

void Document::setActiveLayerIndex(std::size_t index) {
    if (index >= m_impl->layers.size())
        throw std::out_of_range("Layer index out of range");
    m_impl->activeIndex = index;
}

Layer& Document::activeLayer() {
    return layerAt(m_impl->activeIndex);
}

} // namespace skiakrita::core
