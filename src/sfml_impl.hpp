#ifndef SFML_IMPL_HPP_
#define SFML_IMPL_HPP_

#include "chipvm.hpp"

#include <SFML/Graphics.hpp>

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
} // namespace SFMLImpl

#endif /* !SFML_IMPL_HPP_ */
