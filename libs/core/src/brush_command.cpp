/// @file brush_command.cpp
/// @brief BrushStrokeCommand implementation.

#include "core/brush_command.h"
#include "core/layer.h"

namespace skiakrita::core {

BrushStrokeCommand::BrushStrokeCommand(Layer& layer,
                                       std::vector<uint8_t> pixelsBefore)
    : m_layer(&layer)
    , m_before(std::move(pixelsBefore))
{}

BrushStrokeCommand::~BrushStrokeCommand() = default;

void BrushStrokeCommand::captureAfter() {
    m_after = m_layer->pixels();   // copy current (post-stroke) state
}

void BrushStrokeCommand::execute() {
    if (!m_after.empty())
        m_layer->pixels() = m_after;
}

void BrushStrokeCommand::undo() {
    if (!m_before.empty())
        m_layer->pixels() = m_before;
}

std::string BrushStrokeCommand::description() const {
    return "Brush Stroke";
}

} // namespace skiakrita::core
