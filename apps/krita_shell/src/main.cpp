/// @file main.cpp
/// @brief SkiaKrita shell – minimal bootstrap executable.

#include <cmath>
#include <iostream>
#include <string>

#include <core/version.h>
#include <core/document.h>
#include <core/layer.h>
#include <core/brush_engine.h>
#include <core/command.h>
#include <core/selection.h>

#include <graphics_skia/skia_context.h>
#include <graphics_skia/renderer.h>

int main() {
    using namespace skiakrita;

    std::cout << "========================================\n";
    std::cout << "  SkiaKrita  v" << core::Version::toString() << '\n';
    std::cout << "  Build : " << core::Version::getBuildInfo() << '\n';
    std::cout << "========================================\n\n";

    // ── Skia backend info ────────────────────────────────────────
    std::cout << "[graphics] " << graphics::SkiaContext::getBackendInfo() << '\n';

    // ── Legacy test pattern (existing functionality) ─────────────
    graphics::SkiaContext ctx;
    if (!ctx.createSurface(256, 256)) {
        std::cerr << "[error] Failed to create Skia surface.\n";
        return 1;
    }

    const auto info = ctx.getSurfaceInfo();
    std::cout << "[graphics] Surface created: "
              << info.width << 'x' << info.height << '\n';

    if (ctx.drawTestPattern()) {
        std::cout << "[graphics] Test pattern drawn.\n";
    }

    const std::string testPath = "test_output.png";
    if (ctx.saveSnapshot(testPath)) {
        std::cout << "[graphics] Snapshot saved to " << testPath << '\n';
    } else {
        std::cerr << "[warn] Could not save snapshot (non-fatal).\n";
    }

    // ── Document + Layer system ──────────────────────────────────
    std::cout << "\n-- Document / Layer Demo -----------------\n";

    core::Document doc("Untitled", 512, 512);
    std::cout << "[core] Document: " << doc.name()
              << " (" << doc.width() << "x" << doc.height() << ")\n";

    auto& bg = doc.addLayer("Background");
    bg.clear(core::Color::white());
    std::cout << "[core] Layer added: " << bg.name() << '\n';

    auto& paintLayer = doc.addLayer("Paint");
    std::cout << "[core] Layer added: " << paintLayer.name() << '\n';

    std::cout << "[core] Total layers: " << doc.layerCount() << '\n';

    // ── Brush stroke ─────────────────────────────────────────────
    core::BrushStroke stroke;
    stroke.settings.size  = 12.f;
    stroke.settings.color = core::Color::red();

    constexpr float pi2 = 6.2831853f;
    for (float t = 0.f; t <= 1.f; t += 0.02f) {
        const float x = 50.f + t * 400.f;
        const float y = 256.f + std::sin(t * pi2) * 80.f;
        stroke.addPoint(x, y, 0.5f + t * 0.5f);
    }
    std::cout << "[core] Brush stroke: " << stroke.points.size() << " points\n";

    graphics::Renderer renderer;
    graphics::Renderer::renderBrushStroke(paintLayer, stroke);
    std::cout << "[graphics] Stroke rendered to layer.\n";

    // ── Composite and save ───────────────────────────────────────
    if (renderer.compositeDocument(doc)) {
        const std::string compositePath = "composite_output.png";
        if (renderer.saveComposite(compositePath)) {
            std::cout << "[graphics] Composite saved to " << compositePath << '\n';
        }
    }

    // ── Command / Undo demo ──────────────────────────────────────
    std::cout << "\n-- Command / Undo Demo -------------------\n";

    core::CommandHistory history;
    std::cout << "[core] Undo stack: " << history.undoCount()
              << ", Redo stack: " << history.redoCount() << '\n';

    // ── Selection demo ───────────────────────────────────────────
    std::cout << "\n-- Selection Demo ------------------------\n";

    core::Selection sel;
    sel.selectRect({100, 100, 200, 200});
    std::cout << "[core] Selection: "
              << sel.boundingRect().width << "x" << sel.boundingRect().height
              << " at (" << sel.boundingRect().x
              << "," << sel.boundingRect().y << ")\n";
    std::cout << "[core] Contains (150,150): "
              << (sel.contains(150, 150) ? "yes" : "no") << '\n';
    std::cout << "[core] Contains (50,50): "
              << (sel.contains(50, 50) ? "yes" : "no") << '\n';

    std::cout << "\nSkiaKrita bootstrap OK.\n";
    return 0;
}
