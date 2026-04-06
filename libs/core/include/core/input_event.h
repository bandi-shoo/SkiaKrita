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

/// Keyboard modifier flags.
struct KeyModifiers {
    bool ctrl  = false;
    bool shift = false;
    bool alt   = false;
};

/// Virtual key identifiers (extend as needed).
enum class Key { None, Z, Y, S, N, E, Delete, Escape };

/// A single keyboard event.
struct KeyEvent {
    enum class Type { Press, Release };

    Type         type = Type::Press;
    Key          key  = Key::None;
    KeyModifiers mods;
};

} // namespace skiakrita::core
