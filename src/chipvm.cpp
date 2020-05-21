#include "chipvm.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <iostream> // debugging
#include <memory>
#include <random>
#include <stdexcept>
#include <vector>

const std::vector<uint8_t> font{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

ChipVM::ChipVM(
    std::shared_ptr<IDisplayDriver>  display_driver_,
    std::shared_ptr<IKeyboardDriver> keyboard_driver_,
    std::shared_ptr<ISoundDriver>    sound_driver_)
    : ram(0x1000),
      regs(16),
      stack(16),
      display(C8Consts::DISPLAY_WIDTH * C8Consts::DISPLAY_HEIGHT),

      // Drivers initialization
      display_driver(display_driver_),
      keyboard_driver(keyboard_driver_),
      sound_driver(sound_driver_)
{
    std::random_device rd;
    random_gen = std::make_unique<std::mt19937>(rd());

    if (font.size() > ram.size())
        throw std::runtime_error("Font doesn't fit the RAM");

    std::copy(font.cbegin(), font.cend(), ram.begin());
}

bool ChipVM::work()
{
    if (pc + sizeof(instr_t) >= ram.size())
        return false;

    const instr_t instr = fetch_instruction();
    inc_pc();

    /*
    std::cout << "PC: " << std::hex << pc << " | Instruction: " << instr
              << std::endl;
    */

    process_instruction(instr);
    display_driver->render(display);

    return true;
}

void ChipVM::inc_pc() { pc += sizeof(instr_t); }

/*
 * I tried to make it as portable as possible - currently
 * this should work on both LSB and MSB systems.
 *
 * The magic formula inside the loop is just moving
 * a byte N - 1 positions left and perform set that
 * byte in the resulting number.
 */
ChipVM::instr_t ChipVM::fetch_instruction()
{
    instr_t               instr{};
    constexpr std::size_t instr_size = sizeof instr;

    for (std::size_t i = 0; i < instr_size; ++i)
        instr |= ram.at(pc + i) << ((instr_size - i - 1) * CHAR_BIT);

    return instr;
}

decltype(ChipVM::display)::iterator
ChipVM::get_display_pixel(uint8_t x, uint8_t y)
{
    decltype(display)::size_type index = y * C8Consts::DISPLAY_WIDTH + x;

    return index < display.size() ? display.begin() + index : display.end();
}
