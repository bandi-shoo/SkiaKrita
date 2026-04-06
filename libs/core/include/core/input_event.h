#pragma once
/// @file input_event.h
/// @brief Platform-independent input event types.

namespace skiakrita::core {

/// Mouse / stylus button identifiers.
enum class MouseButton { None, Left, Right, Middle };

/// A single mouse or stylus event.
struct MouseEvent {
    enum class Type { Press, Release, Move };

    Type        type     = Type::Move;
    float       x        = 0.f;
    float       y        = 0.f;
    float       pressure = 1.f;   // 1.0 for mouse, variable for tablet
    MouseButton button   = MouseButton::None;
};

} // namespace skiakrita::core
