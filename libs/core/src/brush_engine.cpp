/// @file brush_engine.cpp
/// @brief BrushStroke helper implementation.

#include "core/brush_engine.h"

namespace skiakrita::core {

void BrushStroke::addPoint(float x, float y, float pressure) {
    points.push_back({x, y, pressure});
}

} // namespace skiakrita::core
