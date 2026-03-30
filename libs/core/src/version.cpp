#include "core/version.h"
#include <format>

namespace skiakrita::core {

std::string Version::toString() {
    return std::format("{}.{}.{}-{}", major, minor, patch, label);
}

std::string Version::getBuildInfo() {
    std::string info;
#if defined(_MSC_VER)
    info += std::format("MSVC {}", _MSC_VER);
#elif defined(__clang__)
    info += std::format("Clang {}.{}.{}", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
    info += std::format("GCC {}.{}.{}", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
    info += "Unknown compiler";
#endif

#if defined(_WIN64)
    info += " | Windows x64";
#elif defined(_WIN32)
    info += " | Windows x86";
#elif defined(__APPLE__)
    info += " | macOS";
#elif defined(__linux__)
    info += " | Linux";
#endif

    info += " | C++20";
    return info;
}

} // namespace skiakrita::core
