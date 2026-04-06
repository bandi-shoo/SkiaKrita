#pragma once
/// @file brush_tool.h
/// @brief Brush tool – translates mouse events into rendered brush strokes.

#include <core/tool.h>
#include <core/brush_engine.h>
#include <core/command.h>

#include <functional>
#include <memory>

namespace skiakrita::core {

class Layer;

/// Interactive brush tool.
///
/// On each mouse-move during a drag, the tool creates a tiny two-point
/// stroke (segment) and invokes the RenderCallback so the app layer can
/// render it via graphics::Renderer without core depending on Skia.
class BrushTool : public Tool {
public:
    /// Called for each new segment that needs rendering.
    using RenderCallback  = std::function<void(Layer&, const BrushStroke&)>;
    using CommandCallback = std::function<void(std::unique_ptr<Command>)>;

    BrushTool();
    ~BrushTool() override;

    // Movable, non-copyable.
    BrushTool(BrushTool&&) noexcept;
    BrushTool& operator=(BrushTool&&) noexcept;
    BrushTool(const BrushTool&)            = delete;
    BrushTool& operator=(const BrushTool&) = delete;

    void onMousePress  (Document& doc, const MouseEvent& e) override;
    void onMouseMove   (Document& doc, const MouseEvent& e) override;
    void onMouseRelease(Document& doc, const MouseEvent& e) override;

    [[nodiscard]] std::string name() const override;

    BrushSettings&       settings();
    const BrushSettings& settings() const;

    [[nodiscard]] bool isDrawing() const;

    /// Set the callback invoked for each new stroke segment.
    void setRenderCallback(RenderCallback cb);
    void setCommandCallback(CommandCallback cb);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::core
