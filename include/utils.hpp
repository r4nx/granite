#ifndef UTILS_HPP_
#define UTILS_HPP_

#include <cstdint>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

template <typename... Args>
std::string format_string(const std::string &format, Args... args)
{
    int32_t written = std::snprintf(nullptr, 0, format.c_str(), args...);
    if (written < 0)
        throw std::runtime_error("Failed to format string");

    std::size_t size = written + 1;

    auto buf = std::make_unique<char[]>(size);
    std::snprintf(buf.get(), size, format.c_str(), args...);

    return std::string(buf.get(), buf.get() + size - 1);
}

#endif /* !UTILS_HPP_ */
