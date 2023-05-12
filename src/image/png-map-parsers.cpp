#include "image.hpp"
#include <memory>
#include <cstddef>

void img::PNGImage::parse_8b_truecolor(std::uint8_t*& buffer, size_t size, bool has_alpha) {
    auto step = 3 + has_alpha;
    auto window = _map.columns() * step;
    auto waiting_line = std::make_unique<std::uint8_t[]>(_map.columns() * step);
    for (int row {0}; row < _map.rows(); ++row) {
        if (row > 0) {
            for (int column {0}; column < _map.columns(); ++column) {
                auto red = waiting_line[column * step];
                auto green = waiting_line[column * step + 1];
                auto blue = waiting_line[column * step + 2];
                _map.at(row - 1, column) = img::Color{red, green, blue};
            }
        }
        auto filter = *buffer;
        switch (filter) {
        case 1:
            reverse_sub(buffer + 1, window);
            break;
        case 2:
            reverse_up(buffer + 1, buffer - window, window);
            break;
        case 3:
            reverse_avg(buffer + 1, buffer - window, window);
            break;
        case 4:
            reverse_paeth(buffer + 1, buffer - window, window);
            break;
        case 0:
            break;
        }
        ++buffer;
        for (int i {0}; i < window; ++i) {
            waiting_line[i] = buffer[i];
        }
        buffer += window;
    }
    for (int column {0}; column < _map.columns(); ++column) {
        auto red = waiting_line[column * step];
        auto green = waiting_line[column * step + 1];
        auto blue = waiting_line[column * step + 2];
        _map.at(_map.rows() - 1, column) = img::Color{red, green, blue};
    }
}
