#ifndef COMMON_IMPL_HPP_
#define COMMON_IMPL_HPP_

#include "chipvm.hpp"

#include <vector>

namespace CommonImpl {

class ConsoleDisplayDriver : public IDisplayDriver {
public:
    ConsoleDisplayDriver(char pixel_char_);

    void render(const std::vector<bool> &display) override;

private:
    const char pixel_char{}, empty_char = ' ';
};

} // namespace CommonImpl

#endif /* !COMMON_IMPL_HPP_ */
