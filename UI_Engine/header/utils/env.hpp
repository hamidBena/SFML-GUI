#pragma once
#include <string>
#include <array>
#include <filesystem>

#if defined(_WIN32)
#  include <windows.h>
#elif defined(__linux__)
#  include <unistd.h>
#  include <limits.h>
#endif

namespace env {

inline std::string const& exe_path() {
    static std::string const ret = [] {
        std::string path;
#if defined(_WIN32)
        std::array<char, MAX_PATH> buffer{};
        DWORD length = GetModuleFileNameA(nullptr, buffer.data(), buffer.size());
        if (length != 0) {
            path.assign(buffer.data(), length);
        }
#elif defined(__linux__)
        std::array<char, PATH_MAX> buffer{};
        ssize_t length = ::readlink("/proc/self/exe", buffer.data(), buffer.size());
        if (length != -1) {
            path.assign(buffer.data(), static_cast<std::size_t>(length));
        }
#endif
        return path;
    }();
    return ret;
}

inline std::filesystem::path exe_dir() {
    return std::filesystem::path(exe_path()).parent_path();
}

} // namespace env
