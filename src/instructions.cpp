/*
 * As it's a huge method, it is taken out into separated file.
 */

#include "chipvm.hpp"

#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional> // std::ref
#include <iostream>   // debugging
#include <stdexcept>
#include <thread>
#include <vector>

using namespace std::chrono;

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

                default:
                    goto unk_instruction;
            }
            break;

        // JP addr
        case 0x1:
            pc = instr & 0x0FFF;
            break;

        // CALL addr
        case 0x2:
            try {
                stack.at(++sp) = pc;
                pc             = instr & 0x0FFF;
            }
            catch (const std::out_of_range &) {
                throw std::runtime_error("Stack overflow");
            }
            break;

        // SE Vx, byte
        case 0x3:
        {
            const uint8_t reg  = (instr & 0x0F00) >> 8;
            const uint8_t data = instr & 0x00FF;

            if (regs[reg] == data)
                inc_pc();
            break;
        }

        // SNE Vx, byte
        case 0x4:
        {
            const uint8_t reg  = (instr & 0x0F00) >> 8;
            const uint8_t data = instr & 0x00FF;

            if (regs[reg] != data)
                inc_pc();
            break;
        }

        // SE Vx, Vy
        case 0x5:
        {
            const uint8_t reg_x = (instr & 0x0F00) >> 8,
                          reg_y = (instr & 0x00F0) >> 4;

            if (regs[reg_x] == regs[reg_y])
                inc_pc();
            break;
        }

        // LD Vx, byte
        case 0x6:
        {
            const uint8_t reg  = (instr & 0x0F00) >> 8;
            const uint8_t data = instr & 0x00FF;

            regs[reg] = data;
            break;
        }

        // ADD Vx, byte
        case 0x7:
        {
            const uint8_t reg  = (instr & 0x0F00) >> 8;
            const uint8_t data = instr & 0x00FF;

            regs[reg] += data;
            break;
        }

        // Registers operations
        case 0x8:
        {
            const uint8_t reg_x = (instr & 0x0F00) >> 8,
                          reg_y = (instr & 0x00F0) >> 4;

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

                // SHL
                case 0xE:
                {
                    constexpr uint8_t msb_mask =
                        1U << sizeof(decltype(regs)::value_type) * CHAR_BIT - 1;

                    regs[0xF] = regs[reg_x] & msb_mask;
                    regs[reg_x] <<= 1;
                    break;
                }

                default:
                    goto unk_instruction;
            }
            break;
        }

        case 0x9:
            switch (instr & 0x000F) {
                // SNE Vx, Vy
                case 0x0:
                {
                    const uint8_t reg_x = (instr & 0x0F00) >> 8,
                                  reg_y = (instr & 0x00F0) >> 4;

                    if (reg_x != reg_y)
                        inc_pc();
                    break;
                }

                default:
                    goto unk_instruction;
            }
            break;

        // LD I, addr
        case 0xA:
            i_reg = instr & 0x0FFF;
            break;

        // JP V0, addr
        case 0xB:
            pc = regs[0x0] + (instr & 0x0FFF);
            break;

        // RND Vx, byte
        case 0xC:
        {
            const uint8_t reg  = (instr & 0x0F00) >> 8;
            const uint8_t data = instr & 0x00FF;

            std::uniform_int_distribution<> dis(0, 255);

            regs[reg] = dis(*random_gen) & data;
            break;
        }

        // DRW Vx, Vy, nibble
        case 0xD:
        {
            const uint8_t reg_x = (instr & 0x0F00) >> 8,
                          reg_y = (instr & 0x00F0) >> 4;
            const uint8_t count = instr & 0x000F;

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
            const uint8_t reg = (instr & 0x0F00) >> 8;

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

                default:
                    goto unk_instruction;
            }
            break;
        }

        case 0xF:
        {
            const uint8_t reg = (instr & 0x0F00) >> 8;

            switch (instr & 0x00FF) {
                // LD Vx, DT
                case 0x07:
                    regs[reg] = dt.load();
                    break;

                // LD Vx, K (TO-DO)
                case 0x0A:
                    break;

                /*
                 * It seems that timers are broken, as delay timer didn't pass
                 * some kind of test and Tetris didn't work correctly. I hope
                 * I'll fix them some day. TO-DO.
                 */

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

                    if (i_reg + bcd.size() >= ram.size())
                        throw std::runtime_error("Segmentation fault (too "
                                                 "large BCD representation)");

                    std::reverse(bcd.begin(), bcd.end());
                    std::copy(bcd.begin(), bcd.end(), ram.begin() + i_reg);

                    break;
                }

                // LD [I], Vx
                case 0x55:
                    // Assuming register size equals to RAM cell size
                    if (i_reg + reg >= ram.size())
                        throw std::runtime_error(
                            "Segmentation fault (registers doesn't fit the "
                            "RAM)");

                    std::copy(
                        regs.begin(),
                        regs.begin() + reg,
                        ram.begin() + i_reg);
                    break;

                // LD Vx, [I]
                case 0x65:
                    // Assuming register size equals to RAM cell size
                    if (i_reg + reg >= ram.size())
                        throw std::runtime_error(
                            "Segmentation fault (tried to load registers "
                            "outside the RAM)");

                    std::copy(
                        ram.begin() + i_reg,
                        ram.begin() + i_reg + reg,
                        regs.begin());
                    break;

                default:
                    goto unk_instruction;
            }
            break;
        }

        default:
        unk_instruction:
            std::cout << "[Debug] Unknown instruction: " << std::hex << instr
                      << std::endl;
    }
}
