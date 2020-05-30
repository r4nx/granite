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

#ifndef SFML_IMPL_HPP_
#define SFML_IMPL_HPP_

#include "chipvm.hpp"

#include <SFML/Graphics.hpp>

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace SFMLImpl {
struct Dimensions {
    unsigned width;
    unsigned height;
};

class DisplayDriver : public IDisplayDriver {
public:
    DisplayDriver(
        const std::string &title,
        Dimensions         dim    = {800, 600},
        float              scale_ = 10);

    /*
     * Blocking function, that processes event loop and rendering
     */
    void work();

    void render(const std::vector<bool> &display) override;

private:
    float            scale;
    sf::RenderWindow window;

    std::vector<sf::RectangleShape> pixels;
    std::mutex                      render_mutex;
};

class KeyboardDriver : public IKeyboardDriver {
public:
    bool is_pressed(uint8_t key) override;
};
} // namespace SFMLImpl

#endif /* !SFML_IMPL_HPP_ */
