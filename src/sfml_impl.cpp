#include "sfml_impl.hpp"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

using namespace SFMLImpl;

DisplayDriver::DisplayDriver(const std::string &window_title, Dimensions dim_)
    : window(
        sf::VideoMode(dim_.width, dim_.height),
        window_title,
        sf::Style::Titlebar | sf::Style::Close),
      dim(dim_)
{
    window.setVerticalSyncEnabled(true);

    pixels = std::make_unique<sf::Uint8[]>(
        C8Consts::DISPLAY_WIDTH * C8Consts::DISPLAY_HEIGHT * 4);

    texture = std::make_unique<sf::Texture>();
    texture->create(C8Consts::DISPLAY_WIDTH, C8Consts::DISPLAY_HEIGHT);

    sprite = std::make_unique<sf::Sprite>(*texture);
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
        window.draw(*sprite);
        window.display();
    }
}

void SFMLImpl::DisplayDriver::render(const std::vector<bool> &display)
{
    constexpr struct {
        sf::Uint8 red;
        sf::Uint8 green;
        sf::Uint8 blue;
        sf::Uint8 alpha;
    } pixel_color{0, 0, 0, 255};

    for (std::size_t i = 0; i < display.size(); ++i) {
        const std::size_t pixel_offset = i * 4;

        pixels[pixel_offset + 0] = pixel_color.red;
        pixels[pixel_offset + 1] = pixel_color.green;
        pixels[pixel_offset + 2] = pixel_color.blue;
        pixels[pixel_offset + 3] = display[i] ? pixel_color.alpha : 0;
    }

    texture->update(pixels.get());
}
