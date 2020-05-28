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

#include "common_impl.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

CommonImpl::ConsoleDisplayDriver::ConsoleDisplayDriver(
    char     pixel_char_,
    unsigned render_threshold_)
    : pixel_char(pixel_char_),
      render_threshold(render_threshold_)
{}

void CommonImpl::ConsoleDisplayDriver::render(const std::vector<bool> &display)
{
    /*
     * You will ask me - what's this? This is nothing but what
     * will save you from epilepsy attack lol. Also, without this
     * console refreshes very often and slows down the VM.
     *
     * This should be definitely done in another thread. Let it be TO-DO.
     */
    if (++render_counter >= render_threshold)
        render_counter = 0;
    else
        return;

        // Clear screen
#if defined _WIN32
    system("cls");
#elif defined(__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#endif

    unsigned current_col = 0;

    for (const auto &pixel : display) {
        std::cout << (pixel ? pixel_char : empty_char);

        if (++current_col >= C8Consts::DISPLAY_WIDTH) {
            current_col = 0;
            std::cout << std::endl;
        }
    }
}

void CommonImpl::print_msg(const std::string &msg, MessageType type)
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
