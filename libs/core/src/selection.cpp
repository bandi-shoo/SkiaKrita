/// @file selection.cpp
/// @brief Selection implementation.

#include "core/selection.h"

#include <algorithm>

namespace skiakrita::core {

// ── Rect helpers ─────────────────────────────────────────────────
bool Rect::contains(int px, int py) const {
    return px >= x && px < x + width &&
           py >= y && py < y + height;
}

Rect Rect::intersected(const Rect& other) const {
    const int x1 = std::max(x, other.x);
    const int y1 = std::max(y, other.y);
    const int x2 = std::min(x + width,  other.x + other.width);
    const int y2 = std::min(y + height, other.y + other.height);
    if (x2 <= x1 || y2 <= y1) return {};
    return {x1, y1, x2 - x1, y2 - y1};
}

// ── Pimpl ────────────────────────────────────────────────────────
struct Selection::Impl {
    Rect                 bounds;
    std::vector<uint8_t> maskData;
};

// ── Lifetime ─────────────────────────────────────────────────────
Selection::Selection()  : m_impl(std::make_unique<Impl>()) {}
Selection::~Selection()                                    = default;
Selection::Selection(Selection&&) noexcept                 = default;
Selection& Selection::operator=(Selection&&) noexcept      = default;

// ── Operations ───────────────────────────────────────────────────
void Selection::selectRect(const Rect& rect) {
    if (rect.empty()) { clear(); return; }
    m_impl->bounds = rect;
    m_impl->maskData.assign(
        static_cast<std::size_t>(rect.width) * rect.height, 255);
}

void Selection::selectAll(int width, int height) {
    selectRect({0, 0, width, height});
}

void Selection::clear() {
    m_impl->bounds = {};
    m_impl->maskData.clear();
}

bool Selection::empty() const { return m_impl->maskData.empty(); }

Rect Selection::boundingRect() const { return m_impl->bounds; }

bool Selection::contains(int cx, int cy) const {
    if (!m_impl->bounds.contains(cx, cy)) return false;
    const int lx = cx - m_impl->bounds.x;
    const int ly = cy - m_impl->bounds.y;
    const auto idx =
        static_cast<std::size_t>(ly) * m_impl->bounds.width + lx;
    return idx < m_impl->maskData.size() && m_impl->maskData[idx] > 0;
}

const std::vector<uint8_t>& Selection::mask() const {
    return m_impl->maskData;
}

} // namespace skiakrita::core
