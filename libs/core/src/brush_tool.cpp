/// @file brush_tool.cpp
/// @brief BrushTool implementation.

#include "core/brush_tool.h"
#include "core/brush_command.h"
#include "core/document.h"

namespace skiakrita::core {

// -- Pimpl ----------------------------------------------------------------
struct BrushTool::Impl {
    BrushSettings          settings;
    BrushStroke            currentStroke;
    bool                   drawing = false;
    RenderCallback         renderCb;
    CommandCallback        commandCb;
    std::vector<uint8_t>   pixelSnapshot;
};

// -- Lifetime -------------------------------------------------------------
BrushTool::BrushTool()  : m_impl(std::make_unique<Impl>()) {}
BrushTool::~BrushTool()                                    = default;
BrushTool::BrushTool(BrushTool&&) noexcept                 = default;
BrushTool& BrushTool::operator=(BrushTool&&) noexcept      = default;

// -- Tool interface -------------------------------------------------------
void BrushTool::onMousePress(Document& doc, const MouseEvent& e) {
    if (e.button != MouseButton::Left) return;

    // Snapshot layer pixels before any drawing for undo
    m_impl->pixelSnapshot = doc.activeLayer().pixels();

    m_impl->drawing = true;
    m_impl->currentStroke = {};
    m_impl->currentStroke.settings = m_impl->settings;
    m_impl->currentStroke.addPoint(e.x, e.y, e.pressure);

    // Render initial dab (single-point stroke)
    if (m_impl->renderCb) {
        BrushStroke dab;
        dab.settings = m_impl->settings;
        dab.addPoint(e.x, e.y, e.pressure);
        m_impl->renderCb(doc.activeLayer(), dab);
    }
}

void BrushTool::onMouseMove(Document& doc, const MouseEvent& e) {
    if (!m_impl->drawing) return;

    const Point prev = m_impl->currentStroke.points.back();
    m_impl->currentStroke.addPoint(e.x, e.y, e.pressure);

    // Render only the new segment
    if (m_impl->renderCb) {
        BrushStroke segment;
        segment.settings = m_impl->settings;
        segment.addPoint(prev.x, prev.y, prev.pressure);
        segment.addPoint(e.x, e.y, e.pressure);
        m_impl->renderCb(doc.activeLayer(), segment);
    }
}

void BrushTool::onMouseRelease(Document& doc, const MouseEvent& /*e*/) {
    if (!m_impl->drawing) return;
    m_impl->drawing = false;

    // Create undoable command with before/after pixel snapshots
    if (m_impl->commandCb && !m_impl->pixelSnapshot.empty()) {
        auto cmd = std::make_unique<BrushStrokeCommand>(
            doc.activeLayer(), std::move(m_impl->pixelSnapshot));
        cmd->captureAfter();
        m_impl->commandCb(std::move(cmd));
    }
    m_impl->pixelSnapshot.clear();
}

std::string BrushTool::name() const { return "Brush"; }

// -- Accessors ------------------------------------------------------------
BrushSettings&       BrushTool::settings()       { return m_impl->settings; }
const BrushSettings& BrushTool::settings() const { return m_impl->settings; }

bool BrushTool::isDrawing() const { return m_impl->drawing; }

void BrushTool::setRenderCallback(RenderCallback cb) {
    m_impl->renderCb = std::move(cb);
}

void BrushTool::setCommandCallback(CommandCallback cb) {
    m_impl->commandCb = std::move(cb);
}

} // namespace skiakrita::core
