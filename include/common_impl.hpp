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
