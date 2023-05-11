#include "image.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>

namespace {
int raw(char* buffer, int index) {
    if (index < 0) { return 0; }
    return reinterpret_cast<std::uint8_t&>(buffer[index]);
}
int prior(char* buffer, int index, size_t window) {
    if (index < 0) { return 0; }
    return reinterpret_cast<std::uint8_t&>((buffer - window)[index]);
}
std::uint8_t mod256(int value) {
    while (value < 0) value += 256;
    if (value > 255)  value %= 256;
    std::uint8_t out {static_cast<std::uint8_t>(value)};
    return out;
}
}

void img::PNGImage::apply_sub(char* raw_buffer, size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    char waiting_byte {0};
    for (int i{0}; i < size; ++i) {
        auto result = mod256(raw(raw_buffer, i) - raw(raw_buffer, i - bpp));
        waiting_byte = reinterpret_cast<char&>(result);
        if (i > 0) { raw_buffer[i - 1] = waiting_byte; }
    }
    raw_buffer[size - 1] = waiting_byte;
}

void img::PNGImage::reverse_sub(char* processed_buffer, size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i{0}; i < size; ++i) {
        auto result = mod256(raw(processed_buffer, i) + raw(processed_buffer, i - bpp));
        processed_buffer[i] = reinterpret_cast<char&>(result);
    }
}

void img::PNGImage::apply_up(char* raw_buffer, size_t size) {
    size_t window {_map.columns()};
    auto waiting_line = std::make_unique<char[]>(window);
    for (int row {0}; row < _map.columns(); ++row) {
        if (row > 0) {
            for (int column {0}; column < _map.columns(); ++ column) {
                (raw_buffer - window)[column] = waiting_line[column];
            }
        }
        auto current_line = raw_buffer + window * row;
        for (int column {0}; column < _map.columns(); ++column) {
            auto result = mod256(raw(current_line, column) - prior(current_line, column, window));
            waiting_line[column] = reinterpret_cast<char&>(result);
        }
    }
}

