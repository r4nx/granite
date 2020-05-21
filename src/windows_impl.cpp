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

    auto state = GetAsyncKeyState(key_mapping[key]) & (1U << 15);

    // Debug
    /*
    std::stringstream dbg_msg;
    dbg_msg << "Checked for '" << std::hex << static_cast<uint32_t>(key)
            << "' key, it was " << (state ? "" : "not ") << "pressed";
    MessageBoxA(nullptr, dbg_msg.str().c_str(), "Debug message", MB_OK);
    */

    return state;
}

void WindowsImpl::SoundDriver::beep_for(uint32_t duration)
{
    Beep(750, duration);
}