#include "image.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <cmath>

namespace {
int raw(std::uint8_t* buffer, int index) {
    if (index < 0) { return 0; }
    return buffer[index];
}
int prior(std::uint8_t* buffer, int index) {
    if (index < 0 || !buffer) { return 0; }
    return buffer[index];
}
std::uint8_t mod256(int value) {
    while (value < 0) value += 256;
    if (value > 255)  value %= 256;
    std::uint8_t out {static_cast<std::uint8_t>(value)};
    return out;
}
std::uint8_t paeth_predictor(std::uint8_t left, std::uint8_t up, std::uint8_t up_left) {
    auto p = left +up - up_left;
    auto p_left = abs(p - left);
    auto p_up = abs(p - up);
    auto p_up_left = abs(p - up_left);
    if (p_left <= p_up && p_left <= p_up_left) {
        return left;
    }
    if (p_up < p_up_left) {
        return up;
    }
    return up_left;
}
}

// works for one scanline
void img::PNGImage::apply_sub(std::uint8_t* raw_buffer, size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    std::uint8_t waiting_byte {0};
    for (int i{0}; i < size; ++i) {
        auto result = mod256(raw(raw_buffer, i) - raw(raw_buffer, i - bpp));
        if (i > 0) { raw_buffer[i - 1] = waiting_byte; }
        waiting_byte = result;
    }
    raw_buffer[size - 1] = waiting_byte;
}

// works for one scanline
void img::PNGImage::reverse_sub(std::uint8_t* processed_buffer, size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i{0}; i < size; ++i) {
        auto result = mod256(raw(processed_buffer, i) + raw(processed_buffer, i - bpp));
        processed_buffer[i] = result;
    }
}

void img::PNGImage::apply_up(std::uint8_t* current_buffer,
                             std::uint8_t* upper_buffer,
                             size_t size) {
    for (int i {0}; i < size; ++i) {
        auto result = mod256(raw(current_buffer, i) - prior(upper_buffer, i));
        current_buffer[i] = result;
    }
}

void img::PNGImage::reverse_up(std::uint8_t* processed_buffer,
                               std::uint8_t* upper_buffer,
                               size_t size) {
    for (int i {0}; i < size; ++i) {
        auto result = mod256(raw(processed_buffer, i) + prior(upper_buffer, i));
        processed_buffer[i] = result;
    }
}

void img::PNGImage::apply_avg(std::uint8_t* current_buffer,
                              std::uint8_t* upper_buffer,
                              size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    std::uint8_t waiting_byte {0};
    for (int i {0}; i < size; ++i) {
        auto result = mod256(raw(current_buffer, i) -
                             floor((raw(current_buffer, i - bpp) + prior(upper_buffer, i))));
        if (i > 0) { current_buffer[i - 1] = waiting_byte; }
        waiting_byte = result;
    }
    current_buffer[size - 1] = waiting_byte;
}

void img::PNGImage::reverse_avg(std::uint8_t* current_buffer,
                                std::uint8_t* upper_buffer,
                                size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i {0}; i < size; ++i) {
        auto result = mod256(raw(current_buffer, i) +
                             floor((raw(current_buffer, i - bpp) + prior(upper_buffer, i))));
        current_buffer[i] = result;
    }
}

void img::PNGImage::apply_paeth(std::uint8_t* current_buffer,
                                std::uint8_t* upper_buffer,
                                size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    std::uint8_t waiting_byte {0};
    for (int i {0}; i < size; ++i) {
        auto result = mod256(raw(current_buffer, i) -
                             paeth_predictor(raw(current_buffer, i - bpp),
                                             prior(upper_buffer, i),
                                             prior(upper_buffer, i - bpp)));
        if (i > 0) { current_buffer[i - 1] = waiting_byte; }
        waiting_byte = result;
    }
    current_buffer[size - 1] = waiting_byte;
}

void img::PNGImage::reverse_paeth(std::uint8_t* current_buffer,
                                  std::uint8_t* upper_buffer,
                                  size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i {0}; i < size; ++i) {
        auto result = mod256(raw(current_buffer, i) +
                             paeth_predictor(raw(current_buffer, i - bpp),
                                             prior(upper_buffer, i),
                                             prior(upper_buffer, i - bpp)));
        current_buffer[i] = result;
    }
}
