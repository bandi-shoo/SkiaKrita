#pragma once
/// @file version.h
/// @brief Compile-time and run-time version helpers for SkiaKrita core.

#include <string>
#include <string_view>

namespace skiakrita::core {

struct Version {
    static constexpr int major = 0;
    static constexpr int minor = 1;
    static constexpr int patch = 0;
    static constexpr std::string_view label = "bootstrap";

    /// "0.1.0-bootstrap"
    [[nodiscard]] static std::string toString();

    /// Compiler / platform summary (pure informational).
    [[nodiscard]] static std::string getBuildInfo();
};

} // namespace skiakrita::core
