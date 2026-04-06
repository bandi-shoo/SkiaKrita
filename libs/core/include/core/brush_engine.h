#pragma once
/// @file brush_engine.h
/// @brief Brush settings and stroke data – pure C++20, no rendering deps.

#include <core/color.h>

#include <vector>

namespace skiakrita::core {

/// A single sample point in a stroke (position + pen pressure).
struct Point {
    float x        = 0.f;
    float y        = 0.f;
    float pressure = 1.f;
};

/// Configurable brush parameters.
struct BrushSettings {
    float size     = 10.f;     // diameter in pixels
    float hardness = 1.f;      // 0.0 (soft) … 1.0 (hard)
    float opacity  = 1.f;      // 0.0 … 1.0
    float spacing  = 0.25f;    // fraction of size between dabs
    Color color    = Color::black();
};

/// Accumulated stroke data from a single pen-down → pen-up gesture.
struct BrushStroke {
    BrushSettings      settings;
    std::vector<Point> points;

    [[nodiscard]] bool empty() const { return points.empty(); }
    void addPoint(float x, float y, float pressure = 1.f);
};

} // namespace skiakrita::core
