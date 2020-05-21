#include "common_impl.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
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
