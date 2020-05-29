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
