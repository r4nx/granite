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

#include "sfml_impl.hpp"

#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

using namespace SFMLImpl;

DisplayDriver::DisplayDriver(
    const std::string &window_title,
    Dimensions         dim,
    float              scale_)
    : scale(scale_),
      window(
          sf::VideoMode(dim.width, dim.height),
          window_title,
          sf::Style::Titlebar | sf::Style::Close)
{
    window.setVerticalSyncEnabled(true);
}

void DisplayDriver::work()
{
    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        {
            std::scoped_lock lk(render_mutex);
            for (const auto &pixel : pixels)
                window.draw(pixel);
        }

        window.display();
    }
}

void SFMLImpl::DisplayDriver::render(const std::vector<bool> &display)
{
    std::vector<sf::RectangleShape> temp_pixels;
    sf::Vector2f                    pixel_size{scale, scale};

    for (std::size_t i = 0; i < display.size(); ++i) {
        if (!display[i])
            continue;

        const std::size_t x = i % C8Consts::DISPLAY_WIDTH,
                          y = i / C8Consts::DISPLAY_WIDTH;

        sf::RectangleShape pixel(pixel_size);
        pixel.setFillColor(sf::Color::Black);
        pixel.setPosition({x * scale, y * scale});

        temp_pixels.push_back(pixel);
    }

    {
        std::scoped_lock lk(render_mutex);
        temp_pixels.swap(pixels);
    }
}
