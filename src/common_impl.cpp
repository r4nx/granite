#include "common_impl.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <vector>

CommonImpl::ConsoleDisplayDriver::ConsoleDisplayDriver(char pixel_char_)
    : pixel_char(pixel_char_)
{}

void CommonImpl::ConsoleDisplayDriver::render(const std::vector<bool> &display)
{
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
