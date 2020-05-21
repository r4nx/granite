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

#include "windows_impl.hpp"

#include <Windows.h>

#include <array>
#include <cstdint>
#include <sstream>

const static std::array<uint32_t, 16> key_mapping{
    VK_NUMPAD0,  // 0
    VK_NUMPAD7,  // 1
    VK_NUMPAD8,  // 2
    VK_NUMPAD9,  // 3
    VK_NUMPAD4,  // 4
    VK_NUMPAD5,  // 5
    VK_NUMPAD6,  // 6
    VK_NUMPAD1,  // 7
    VK_NUMPAD2,  // 8
    VK_NUMPAD3,  // 9
    VK_DIVIDE,   // A
    VK_MULTIPLY, // B
    VK_SUBTRACT, // C
    VK_ADD,      // D
    VK_OEM_PLUS, // E
    VK_DECIMAL}; // F

bool WindowsImpl::KeyboardDriver::is_pressed(uint8_t key)
{
    if (key >= key_mapping.size()) {
        std::stringstream error_msg;
        error_msg << "Key was not found in mapping: " << std::hex << key;

        MessageBoxA(
            nullptr,
            error_msg.str().c_str(),
            "Error",
            MB_OK | MB_ICONERROR);

        return false;
    }

    return GetAsyncKeyState(key_mapping[key]) & (1U << 15);
}

void WindowsImpl::SoundDriver::beep_for(uint32_t duration)
{
    Beep(750, duration);
}