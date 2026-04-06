#pragma once
/// @file brush_command.h
/// @brief Undoable command for a brush stroke (pixel-snapshot approach).

#include <core/command.h>

#include <cstdint>
#include <vector>

namespace skiakrita::core {

class Layer;

/// Captures layer pixel state before / after a stroke for undo / redo.
class BrushStrokeCommand : public Command {
public:
    /// Construct with the target layer and a pixel snapshot taken BEFORE
    /// any drawing.  Call captureAfter() once the stroke is complete.
    BrushStrokeCommand(Layer& layer, std::vector<uint8_t> pixelsBefore);
    ~BrushStrokeCommand() override;

    /// Snapshot the current layer pixels as the "after" state.
    void captureAfter();

    void execute() override;   // redo – restores "after" pixels
    void undo()    override;   // restores "before" pixels

    [[nodiscard]] std::string description() const override;

private:
    Layer*               m_layer;
    std::vector<uint8_t> m_before;
    std::vector<uint8_t> m_after;
};

} // namespace skiakrita::core
