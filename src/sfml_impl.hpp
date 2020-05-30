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

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <vector>

namespace SFMLImpl {
struct Dimensions {
    unsigned width;
    unsigned height;
};

class DisplayDriver : public IDisplayDriver {
    using key_press_callback_t = std::function<void(sf::Keyboard::Key)>;

public:
    DisplayDriver(
        const std::string &title,
        Dimensions         dim    = {800, 600},
        float              scale_ = 10);

    /*
     * Blocking function, that processes event loop and rendering
     */
    void work();
    void subscribe_for_key_press(key_press_callback_t callback);

    void render(const std::vector<bool> &display) override;

private:
    float            scale;
    sf::RenderWindow window;

    std::vector<sf::RectangleShape> pixels;
    std::mutex                      render_mutex;

    std::vector<key_press_callback_t> key_press_subscribers;
};

class KeyboardDriver : public IKeyboardDriver {
public:
    void press_callback(sf::Keyboard::Key key);

    bool    is_pressed(uint8_t key) override;
    uint8_t wait_for_key() override;
    void    shutdown() override;

private:
    std::condition_variable cv;
    std::mutex              key_press_mut;

    uint8_t last_pressed_key = 0;
};
} // namespace SFMLImpl

#endif /* !SFML_IMPL_HPP_ */
