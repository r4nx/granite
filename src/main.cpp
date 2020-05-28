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

#include "chipvm.hpp"
#include "common_impl.hpp"
#include "windows_impl.hpp"

#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <functional>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

using CommonImpl::MessageType;

bool load_image(std::shared_ptr<ChipVM> vm, const std::string &file_name)
{
    try {
        using ram_cell_t       = decltype(vm->ram)::value_type;
        using input_iterator_t = std::istreambuf_iterator<char>;

        std::ifstream image_file;

        image_file.exceptions(
            image_file.exceptions() | std::ios::failbit | std::ios::badbit);
        image_file.open(file_name, std::ios::binary | std::ios::ate);

        // Check whether the image size is less than RAM size
        auto               ram_size   = vm->ram.size() * sizeof(ram_cell_t);
        decltype(ram_size) image_size = image_file.tellg();

        if (image_size > ram_size - C8Consts::USER_SPACE) {
            print_msg("Image doesn't fit the RAM.", MessageType::error);
            return false;
        }

        image_file.seekg(0, std::ios::beg);

        // I am a paranoid lol
        static_assert(
            sizeof(input_iterator_t::value_type) == sizeof(ram_cell_t),
            "istreambuf_iterator type have to be the same length as VM's RAM "
            "cell (this should have never happened, basically it's an "
            "assertion that "
            "sizeof(char) == sizeof(uint_8)).");

        // Finally load image to RAM, starting from USER_SPACE
        std::copy(
            input_iterator_t(image_file),
            input_iterator_t(),
            vm->ram.begin() + C8Consts::USER_SPACE);
    }
    catch (const std::ios_base::failure &) {
        CommonImpl::print_msg(
            std::string("Failed to read image file:\n\t")
                + std::strerror(errno),
            MessageType::error);
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{

    if (argc < 2) {
        print_msg(
            "Please specify chip8 image as an argument.",
            MessageType::error);
        return 0;
    }

    // Initialize drivers and the virtual machine itself
    auto display_driver =
        std::make_shared<CommonImpl::ConsoleDisplayDriver>('W');
    auto keyboard_driver = std::make_shared<WindowsImpl::KeyboardDriver>();
    auto sound_driver    = std::make_shared<WindowsImpl::SoundDriver>();
    auto vm =
        std::make_shared<ChipVM>(display_driver, keyboard_driver, sound_driver);

    if (!load_image(vm, argv[1]))
        return 1;

    // Work on the VM
    std::thread vm_thread([vm] {
        try {
            while (vm->work()) {}
        }
        catch (const std::runtime_error &ex) {
            print_msg(
                std::string("Runtime error:\n\t") + ex.what(),
                MessageType::error);
        }
    });

    vm_thread.join();

    return 0;
}
