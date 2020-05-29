#ifndef SFML_IMPL_HPP_
#define SFML_IMPL_HPP_

#include "chipvm.hpp"

#include <SFML/Graphics.hpp>

#include <memory>
#include <string>
#include <vector>

namespace SFMLImpl {
struct Dimensions {
    unsigned width;
    unsigned height;
};

class DisplayDriver : public IDisplayDriver {
public:
    DisplayDriver(const std::string &title, Dimensions dim_ = {800, 600});

    /*
     * Blocking function, that processes event loop and rendering
     */
    void work();

    void render(const std::vector<bool> &display) override;

private:
    sf::RenderWindow window;
    Dimensions       dim;

    std::unique_ptr<sf::Uint8[]> pixels;
    std::unique_ptr<sf::Texture> texture;
    std::unique_ptr<sf::Sprite>  sprite;
};
} // namespace SFMLImpl

#endif /* !SFML_IMPL_HPP_ */
