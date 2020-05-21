#ifndef WINDOWS_IMPL_HPP_
#define WINDOWS_IMPL_HPP_

#include "chipvm.hpp"

#include <Windows.h>

#include <array>
#include <cstdint>

namespace WindowsImpl {

class KeyboardDriver : public IKeyboardDriver {
public:
    bool is_pressed(uint8_t key) override;
};

class SoundDriver : public ISoundDriver {
public:
    void beep_for(uint32_t duration) override;
};

} // namespace WindowsImpl

#endif /* !WINDOWS_IMPL_HPP_ */
