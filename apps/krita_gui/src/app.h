#pragma once
/// @file app.h
/// @brief Application controller – wires Document, BrushTool, Renderer,
///        and Win32Window together.

#include <memory>

namespace skiakrita::gui {

class App {
public:
    App();
    ~App();

    App(const App&)            = delete;
    App& operator=(const App&) = delete;

    /// Show window and enter the message loop (blocks).
    int run();

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::gui
