#pragma once
/// @file selection.h
/// @brief Rectangular selection region with alpha mask.

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

namespace skiakrita::core {

/// Axis-aligned integer rectangle.
struct Rect {
    int x      = 0;
    int y      = 0;
    int width  = 0;
    int height = 0;

    [[nodiscard]] bool empty() const { return width <= 0 || height <= 0; }
    [[nodiscard]] bool contains(int px, int py) const;
    [[nodiscard]] Rect intersected(const Rect& other) const;
};

/// Selection region represented by a bounding rect + alpha mask.
class Selection {
public:
    Selection();
    ~Selection();

    // Movable, non-copyable.
    Selection(Selection&&) noexcept;
    Selection& operator=(Selection&&) noexcept;
    Selection(const Selection&)            = delete;
    Selection& operator=(const Selection&) = delete;

    void selectRect(const Rect& rect);
    void selectAll(int width, int height);
    void clear();

    [[nodiscard]] bool empty() const;
    [[nodiscard]] Rect boundingRect() const;
    [[nodiscard]] bool contains(int x, int y) const;

    /// Per-pixel mask (0 = not selected, 255 = fully selected).
    [[nodiscard]] const std::vector<uint8_t>& mask() const;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::core
