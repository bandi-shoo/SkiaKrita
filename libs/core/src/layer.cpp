/// @file layer.cpp
/// @brief Layer implementation.

#include "core/layer.h"

#include <algorithm>

namespace skiakrita::core {

// ── Pimpl ────────────────────────────────────────────────────────
struct Layer::Impl {
    std::string          name;
    LayerType            type;
    int                  width;
    int                  height;
    bool                 visible   = true;
    float                opacity   = 1.f;
    BlendMode            blendMode = BlendMode::Normal;
    std::vector<uint8_t> pixelData;  // premultiplied RGBA8
};

// ── Lifetime ─────────────────────────────────────────────────────
Layer::Layer(std::string name, LayerType type, int width, int height)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->name   = std::move(name);
    m_impl->type   = type;
    m_impl->width  = width;
    m_impl->height = height;
    m_impl->pixelData.resize(
        static_cast<std::size_t>(width) * height * 4, 0);
}

Layer::~Layer()                                    = default;
Layer::Layer(Layer&&) noexcept                     = default;
Layer& Layer::operator=(Layer&&) noexcept          = default;

// ── Accessors ────────────────────────────────────────────────────
const std::string& Layer::name()  const { return m_impl->name; }
void Layer::setName(std::string n)      { m_impl->name = std::move(n); }

LayerType Layer::type() const { return m_impl->type; }

bool  Layer::visible()  const      { return m_impl->visible; }
void  Layer::setVisible(bool v)    { m_impl->visible = v; }

float Layer::opacity()  const      { return m_impl->opacity; }
void  Layer::setOpacity(float o)   { m_impl->opacity = std::clamp(o, 0.f, 1.f); }

BlendMode Layer::blendMode()  const        { return m_impl->blendMode; }
void      Layer::setBlendMode(BlendMode m) { m_impl->blendMode = m; }

int Layer::width()  const { return m_impl->width; }
int Layer::height() const { return m_impl->height; }

const std::vector<uint8_t>& Layer::pixels() const { return m_impl->pixelData; }
std::vector<uint8_t>&       Layer::pixels()       { return m_impl->pixelData; }

void Layer::clear(Color c) {
    auto& px = m_impl->pixelData;
    // Store as premultiplied RGBA
    const float a = c.a / 255.f;
    const uint8_t pr = static_cast<uint8_t>(c.r * a);
    const uint8_t pg = static_cast<uint8_t>(c.g * a);
    const uint8_t pb = static_cast<uint8_t>(c.b * a);
    for (std::size_t i = 0; i < px.size(); i += 4) {
        px[i]     = pr;
        px[i + 1] = pg;
        px[i + 2] = pb;
        px[i + 3] = c.a;
    }
}

} // namespace skiakrita::core
