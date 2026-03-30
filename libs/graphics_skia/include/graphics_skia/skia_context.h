#pragma once
/// @file skia_context.h
/// @brief Platform-independent Skia wrapper – public API is pure C++20.
///
/// Implementation details (Skia types) are hidden behind the pimpl idiom
/// so that consumers of this library never need Skia headers.

#include <memory>
#include <string>

namespace skiakrita::graphics {

/// Basic information about a raster surface.
struct SurfaceInfo {
    int  width  = 0;
    int  height = 0;
    bool valid  = false;
};

/// Thin wrapper around a Skia raster surface.
class SkiaContext {
public:
    SkiaContext();
    ~SkiaContext();

    // Movable, non-copyable.
    SkiaContext(SkiaContext&&) noexcept;
    SkiaContext& operator=(SkiaContext&&) noexcept;
    SkiaContext(const SkiaContext&)            = delete;
    SkiaContext& operator=(const SkiaContext&) = delete;

    /// Human-readable Skia backend description.
    [[nodiscard]] static std::string getBackendInfo();

    /// Create an RGBA raster surface of the given size.
    bool createSurface(int width, int height);

    /// Query the current surface state.
    [[nodiscard]] SurfaceInfo getSurfaceInfo() const;

    /// Draw a simple test pattern (rectangles, circle, line).
    bool drawTestPattern();

    /// Encode the current surface to a PNG file.
    bool saveSnapshot(const std::string& filePath);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace skiakrita::graphics
