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
