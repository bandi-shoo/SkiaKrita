/// @file color.cpp
/// @brief BlendMode utility implementation.

#include "core/color.h"

namespace skiakrita::core {

std::string blendModeToString(BlendMode mode) {
    switch (mode) {
        case BlendMode::Normal:    return "Normal";
        case BlendMode::Multiply:  return "Multiply";
        case BlendMode::Screen:    return "Screen";
        case BlendMode::Overlay:   return "Overlay";
        case BlendMode::Darken:    return "Darken";
        case BlendMode::Lighten:   return "Lighten";
        case BlendMode::SoftLight: return "SoftLight";
        case BlendMode::HardLight: return "HardLight";
    }
    return "Unknown";
}

} // namespace skiakrita::core
