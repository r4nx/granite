#ifndef CHIPVM_HPP_
#define CHIPVM_HPP_

#include <atomic>
#include <cstdint>
#include <memory>
#include <random>
#include <vector>

namespace C8Consts {
enum {
    DISPLAY_WIDTH    = 64,
    DISPLAY_HEIGHT   = 32,
    INSTRUCTION_LEN  = 0x2,
    USER_SPACE       = 0x200,
    SPRITE_WIDTH     = 8,
    FONT_CHAR_SIZE   = 5,
    TIMERS_FREQUENCY = 60
};
}

class IDisplayDriver;

class ChipVM {
    using instr_t = uint16_t;

    friend class IDisplayDriver;
    friend class IKeyboardDriver;
    friend class ISoundDriver;

public:
    ChipVM(
        std::shared_ptr<IDisplayDriver>  display_driver_,
        std::shared_ptr<IKeyboardDriver> keyboard_driver_,
        std::shared_ptr<ISoundDriver>    sound_driver_);

    /**
     * The most important method, that is fetching and executing instructions,
     * drawing the display, etc.
     *
     * \return Has VM finished its work
     */
    bool work();

    std::vector<uint8_t>  ram, regs;
    std::vector<uint16_t> stack;
    std::vector<bool>     display;

    // Special registers
    uint16_t             pc    = C8Consts::USER_SPACE; // program counter
    uint16_t             i_reg = 0;                    // index register
    uint8_t              sp    = 0;                    // stack pointer
    std::atomic<uint8_t> dt = 0, st = 0;               // delay and sound timers

    std::shared_ptr<IDisplayDriver>  display_driver;
    std::shared_ptr<IKeyboardDriver> keyboard_driver;
    std::shared_ptr<ISoundDriver>    sound_driver;

private:
    void                        inc_pc();
    instr_t                     fetch_instruction();
    void                        process_instruction(instr_t instr);
    decltype(display)::iterator get_display_pixel(uint8_t x, uint8_t y);

    std::unique_ptr<std::mt19937> random_gen;
    unsigned                      render_counter = 0; // debug
};

/*
 * ==================
 * Drivers interfaces
 * ==================
 */

class IDisplayDriver {
public:
    virtual void render(const std::vector<bool> &display) = 0;
};

class IKeyboardDriver {
public:
    virtual bool is_pressed(uint8_t key) = 0;
};

class ISoundDriver {
public:
    virtual void beep_for(uint32_t duration) = 0;
};

#endif /* !CHIPVM_HPP_ */
