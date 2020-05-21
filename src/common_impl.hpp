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

#ifndef COMMON_IMPL_HPP_
#define COMMON_IMPL_HPP_

#include "chipvm.hpp"

#include <vector>

namespace CommonImpl {

class ConsoleDisplayDriver : public IDisplayDriver {
public:
    ConsoleDisplayDriver(char pixel_char_, unsigned render_threshold_ = 250);

    void render(const std::vector<bool> &display) override;

private:
    const char pixel_char{}, empty_char = ' ';

    const unsigned render_threshold = 0;
    unsigned       render_counter   = 0;
};

} // namespace CommonImpl

#endif /* !COMMON_IMPL_HPP_ */
