/// @file main.cpp
/// @brief SkiaKrita shell – minimal bootstrap executable.

#include <iostream>
#include <string>

#include <core/version.h>
#include <graphics_skia/skia_context.h>

int main() {
    using namespace skiakrita;

    std::cout << "========================================\n";
    std::cout << "  SkiaKrita  v" << core::Version::toString() << '\n';
    std::cout << "  Build : " << core::Version::getBuildInfo() << '\n';
    std::cout << "========================================\n\n";

    // ── Skia backend info ────────────────────────────────────────
    std::cout << "[graphics] " << graphics::SkiaContext::getBackendInfo() << '\n';

    // ── Create a small raster surface and draw a test pattern ────
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

    const std::string outPath = "test_output.png";
    if (ctx.saveSnapshot(outPath)) {
        std::cout << "[graphics] Snapshot saved to " << outPath << '\n';
    } else {
        std::cerr << "[warn] Could not save snapshot (non-fatal).\n";
    }

    std::cout << "\nSkiaKrita bootstrap OK.\n";
    return 0;
}
