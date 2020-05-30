/*
 * granite - CHIP-8 emulator
 *  Copyright (C) 2020 r4nx
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#ifdef _WIN32
#include <Windows.h>
#else
#include <iostream>
#endif

#include <cstdint>
#include <cstdio>
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

enum class MessageType { info, error };
void print_msg(const std::string &msg, MessageType type)
{
#ifdef _WIN32
    MessageBoxA(
        nullptr,
        msg.c_str(),
        "granite",
        MB_OK
            | (type == MessageType::error ? MB_ICONERROR : MB_ICONINFORMATION));
#else
    (type == MessageType::error ? std::cerr : std::cout) << msg << std::endl;
#endif
}

#endif /* !UTILS_HPP_ */
