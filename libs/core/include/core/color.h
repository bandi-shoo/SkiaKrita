#pragma once
/// @file color.h
/// @brief Color types and blend modes for SkiaKrita core.

#include <cstdint>
#include <string>

namespace skiakrita::core {

/// RGBA color value (8-bit per channel).
struct Color {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;

    [[nodiscard]] static constexpr Color black()       { return {0, 0, 0, 255}; }
    [[nodiscard]] static constexpr Color white()       { return {255, 255, 255, 255}; }
    [[nodiscard]] static constexpr Color red()         { return {255, 0, 0, 255}; }
    [[nodiscard]] static constexpr Color green()       { return {0, 255, 0, 255}; }
    [[nodiscard]] static constexpr Color blue()        { return {0, 0, 255, 255}; }
    [[nodiscard]] static constexpr Color transparent() { return {0, 0, 0, 0}; }

    [[nodiscard]] bool operator==(const Color&) const = default;
};

/// Layer compositing blend modes.
enum class BlendMode {
    Normal,
    Multiply,
    Screen,
    Overlay,
    Darken,
    Lighten,
    SoftLight,
    HardLight,
};

/// Convert a BlendMode to a human-readable string.
[[nodiscard]] std::string blendModeToString(BlendMode mode);

} // namespace skiakrita::core
