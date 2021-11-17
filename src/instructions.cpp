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

// As it's a huge method, it is taken out into separated file.

#include "chipvm.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional> // std::ref
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std::chrono;
using instr_t = ChipVM::instr_t;

uint16_t decode_addr(instr_t instr) noexcept { return instr & 0xFFF; }
uint8_t  decode_reg_x(instr_t instr) noexcept { return (instr & 0x0F00) >> 8; }
uint8_t  decode_reg_y(instr_t instr) noexcept { return (instr & 0x00F0) >> 4; }
uint8_t  decode_imm(instr_t instr) noexcept { return instr & 0x00FF; }
uint8_t  decode_nibble(instr_t instr) noexcept { return instr & 0x000F; }

void ChipVM::process_instruction(const instr_t instr)
{
    switch ((instr & 0xF000) >> 12) {
        case 0x0:
            switch (instr & 0x00FF) {
                // CLS
                case 0xE0:
                    std::fill(display.begin(), display.end(), 0);
                    break;

                // RET
                case 0xEE:
                    try {
                        pc = stack.at(sp--);
                    }
                    catch (const std::out_of_range &) {
                        throw std::runtime_error("Stack underflow");
                    }
                    break;
            }
            break;

        // JP addr
        case 0x1:
            pc = decode_addr(instr);
            break;

        // CALL addr
        case 0x2:
            try {
                stack.at(++sp) = pc;
                pc             = decode_addr(instr);
            }
            catch (const std::out_of_range &) {
                throw std::runtime_error("Stack overflow");
            }
            break;

        // SE Vx, byte
        case 0x3:
        {
            const uint8_t reg  = decode_reg_x(instr);
            const uint8_t data = decode_imm(instr);

            if (regs[reg] == data)
                inc_pc();
            break;
        }

        // SNE Vx, byte
        case 0x4:
        {
            const uint8_t reg  = decode_reg_x(instr);
            const uint8_t data = decode_imm(instr);

            if (regs[reg] != data)
                inc_pc();
            break;
        }

        // SE Vx, Vy
        case 0x5:
        {
            const uint8_t reg_x = decode_reg_x(instr),
                          reg_y = decode_reg_y(instr);

            if (regs[reg_x] == regs[reg_y])
                inc_pc();
            break;
        }

        // LD Vx, byte
        case 0x6:
        {
            const uint8_t reg  = decode_reg_x(instr);
            const uint8_t data = decode_imm(instr);

            regs[reg] = data;
            break;
        }

        // ADD Vx, byte
        case 0x7:
        {
            const uint8_t reg  = decode_reg_x(instr);
            const uint8_t data = decode_imm(instr);

            regs[reg] += data;
            break;
        }

        // Registers operations
        case 0x8:
        {
            const uint8_t reg_x = decode_reg_x(instr),
                          reg_y = decode_reg_y(instr);

            switch (instr & 0x000F) {
                // LD Vx, Vy
                case 0x0:
                    regs[reg_x] = regs[reg_y];
                    break;

                // OR Vx, Vy
                case 0x1:
                    regs[reg_x] |= regs[reg_y];
                    break;

                // AND Vx, Vy
                case 0x2:
                    regs[reg_x] &= regs[reg_y];
                    break;

                // XOR Vx, Vy
                case 0x3:
                    regs[reg_x] ^= regs[reg_y];
                    break;

                // ADD Vx, Vy
                case 0x4:
                {
                    const uint16_t wide_sum = regs[reg_x] + regs[reg_y];

                    regs[0xF] = wide_sum > UINT8_MAX ? 1 : 0;
                    regs[reg_x] += regs[reg_y];
                    break;
                }

                // SUB Vx, Vy
                case 0x5:
                    regs[0xF] = regs[reg_x] > regs[reg_y] ? 1 : 0;
                    regs[reg_x] -= regs[reg_y];
                    break;

                // SHR
                case 0x6:
                    regs[0xF] = regs[reg_x] & 0xF;
                    regs[reg_x] >>= 1;
                    break;

                // SUBN Vx, Vy
                case 0x7:
                    regs[0xF]   = regs[reg_y] > regs[reg_x] ? 1 : 0;
                    regs[reg_x] = regs[reg_y] - regs[reg_x];
                    break;

                // SHL Vx
                case 0xE:
                {
                    constexpr uint8_t msb_mask =
                        1U << sizeof(decltype(regs)::value_type) * CHAR_BIT - 1;

                    regs[0xF] = regs[reg_x] & msb_mask;
                    regs[reg_x] <<= 1;
                    break;
                }
            }
            break;
        }

        case 0x9:
            switch (instr & 0x000F) {
                // SNE Vx, Vy
                case 0x0:
                {
                    const uint8_t reg_x = decode_reg_x(instr),
                                  reg_y = decode_reg_y(instr);

                    if (regs[reg_x] != regs[reg_y])
                        inc_pc();
                    break;
                }
            }
            break;

        // LD I, addr
        case 0xA:
            i_reg = decode_addr(instr);
            break;

        // JP V0, addr
        case 0xB:
            pc = regs[0x0] + decode_addr(instr);
            break;

        // RND Vx, byte
        case 0xC:
        {
            const uint8_t reg  = decode_reg_x(instr);
            const uint8_t data = decode_imm(instr);

            std::uniform_int_distribution<> dis(0, 255);

            regs[reg] = dis(*random_gen) & data;
            break;
        }

        // DRW Vx, Vy, nibble
        case 0xD:
        {
            const uint8_t reg_x = decode_reg_x(instr),
                          reg_y = decode_reg_y(instr),
                          count = decode_nibble(instr);

            uint8_t x = regs[reg_x], y = regs[reg_y];

            if (i_reg + count >= ram.size())
                throw std::runtime_error(
                    "Segmentation fault (sprite is out of range of RAM)");

            /*
             * As src (see below) denotes byte in RAM (which is typically is 8
             * bit), bit shifting, that fetches pixel bit, will fail, because
             * maximum value the byte can be shifted right is SPRITE_WIDTH - 1.
             */
            static_assert(
                sizeof(decltype(ram)::value_type) * CHAR_BIT
                    == C8Consts::SPRITE_WIDTH,
                "Sprite width is not equal to RAM cell size: the rendering may "
                "not work as expected. See source code comments for details.");

            const auto src_beg = ram.begin() + i_reg;
            auto       src     = src_beg;
            auto       dst     = display.end();

            /*
             * src denotes bits, packed into byte
             * dst denotes bool
             *
             * Range-safety of src is ensured by the if check at the
             * beginning of instruction processing routine, safety of dst -
             * by get_display_pixel method - it returnes off-the-end
             * iterator if desired pixel is out of range.
             */

            regs[0xF] = 0;
            while (src != src_beg + count) {
                dst = get_display_pixel(x, y);

                for (std::size_t i = C8Consts::SPRITE_WIDTH; i > 0; --i) {
                    if (dst == display.end())
                        break;

                    // Fetch i - 1 bit from the right
                    bool src_pixel = (*src >> (i - 1)) & 1U;

                    // If pixel erased
                    if (*dst && src_pixel)
                        regs[0xF] = 1;

                    // XOR pixel, according to the specification
                    *dst = *dst ^ src_pixel;
                    ++dst;
                }

                // If above loop was finished due to end of display
                if (dst == display.end())
                    break;

                ++y;
                ++src;
            }

            break;
        }

        case 0xE:
        {
            const uint8_t reg = decode_reg_x(instr);

            switch (instr & 0x00FF) {
                // SKP Vx
                case 0x9E:
                    if (keyboard_driver->is_pressed(regs[reg]))
                        inc_pc();
                    break;

                // SKNP Vx
                case 0xA1:
                    if (!keyboard_driver->is_pressed(regs[reg]))
                        inc_pc();
                    break;
            }
            break;
        }

        case 0xF:
        {
            const uint8_t reg = decode_reg_x(instr);

            switch (instr & 0x00FF) {
                // LD Vx, DT
                case 0x07:
                    regs[reg] = dt.load();
                    break;

                // LD Vx, K
                case 0x0A:
                    regs[reg] = keyboard_driver->wait_for_key();
                    break;

                // LD DT, Vx
                case 0x15:
                {
                    bool thread_started = dt.load() > 0;

                    dt.store(regs[reg]);

                    if (!thread_started) {
                        std::thread delay_timer_thread(
                            [](std::atomic<uint8_t> &dt) {
                                while (dt.load() > 0) {
                                    --dt;

                                    std::this_thread::sleep_for(milliseconds(
                                        1000 / C8Consts::TIMERS_FREQUENCY));
                                }
                            },
                            std::ref(dt));

                        delay_timer_thread.detach();
                    }
                    break;
                }

                // LD ST, Vx
                case 0x18:
                {
                    bool thread_started = st.load() > 0;

                    st.store(regs[reg]);

                    if (!thread_started) {
                        std::thread sound_timer_thread(
                            [](std::atomic<uint8_t> &        st,
                               std::shared_ptr<ISoundDriver> sound_driver) {
                                constexpr uint32_t frequency =
                                    1000 / C8Consts::TIMERS_FREQUENCY;

                                while (st.load() > 0) {
                                    --st;
                                    sound_driver->beep_for(frequency);

                                    std::this_thread::sleep_for(
                                        milliseconds(frequency));
                                }
                            },
                            std::ref(st),
                            sound_driver);

                        sound_timer_thread.detach();
                    }
                    break;
                }

                // ADD I, Vx
                case 0x1E:
                    i_reg += regs[reg];
                    break;

                // LD F, Vx
                case 0x29:
                    // Fonts are stored at the beginning of RAM
                    i_reg = regs[reg] * C8Consts::FONT_CHAR_SIZE;
                    break;

                // LD B, Vx
                case 0x33:
                {
                    uint8_t              n = regs[reg];
                    std::vector<uint8_t> bcd;

                    while (n > 0) {
                        bcd.push_back(n % 10);
                        n /= 10;
                    }

                    // Hundreds, tens and ones
                    constexpr decltype(bcd)::size_type digits = 3;
                    while (bcd.size() < digits)
                        bcd.push_back(0);

                    if (i_reg + bcd.size() >= ram.size())
                        throw std::runtime_error("Segmentation fault (too "
                                                 "large BCD representation)");

                    std::reverse(bcd.begin(), bcd.end());
                    std::copy(bcd.begin(), bcd.end(), ram.begin() + i_reg);

                    break;
                }

                // LD [I], Vx
                case 0x55:
                {
                    // + 1 is needed because specification says to save/load
                    // registers V0 THROUGH Vx (inclusively)
                    const uint8_t regs_count = reg + 1;

                    // Assuming register size equals to RAM cell size
                    if (i_reg + regs_count >= ram.size())
                        throw std::runtime_error(
                            "Segmentation fault (registers doesn't fit the "
                            "RAM)");

                    std::copy_n(regs.begin(), regs_count, ram.begin() + i_reg);
                    break;
                }

                // LD Vx, [I]
                case 0x65:
                {
                    const uint8_t regs_count = reg + 1;

                    // Assuming register size equals to RAM cell size
                    if (i_reg + regs_count >= ram.size())
                        throw std::runtime_error(
                            "Segmentation fault (tried to load registers "
                            "outside the RAM)");

                    std::copy_n(ram.begin() + i_reg, regs_count, regs.begin());
                    break;
                }
            }
            break;
        }
    }
}
