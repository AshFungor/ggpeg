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
int paeth_predictor(std::uint8_t a, std::uint8_t b, std::uint8_t c) {
    int p = a + b - c;
    auto p_a = abs(p - a);
    auto p_b = abs(p - b);
    auto p_c = abs(p - c);
    if (p_a <= p_b && p_a <= p_c) {
        return a;
    }
    if (p_b <= p_c && p_b <= p_a) {
        return b;
    }
    return c;
}
}

// works for one scanline
void img::PNGImage::apply_sub(std::uint8_t* raw_buffer, size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    auto filtered = std::make_unique<std::uint8_t[]>(size);
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(raw_buffer, i) - raw(raw_buffer, i - bpp);
        filtered[i] = result;
    }
    for (size_t i {0}; i < size; ++i) {
        raw_buffer[i] = filtered[i];
    }
}

// works for one scanline
void img::PNGImage::reverse_sub(std::uint8_t* processed_buffer, size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i{0}; i < size; ++i) {
        std::uint8_t result = raw(processed_buffer, i) + raw(processed_buffer, i - bpp);
        processed_buffer[i] = result;
    }
}

void img::PNGImage::apply_up(std::uint8_t* current_buffer,
                             std::uint8_t* upper_buffer,
                             size_t size) {
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(current_buffer, i) - prior(upper_buffer, i);
        current_buffer[i] = result;
    }
}

void img::PNGImage::reverse_up(std::uint8_t* processed_buffer,
                               std::uint8_t* upper_buffer,
                               size_t size) {
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(processed_buffer, i) + prior(upper_buffer, i);
        processed_buffer[i] = result;
    }
}

void img::PNGImage::apply_avg(std::uint8_t* current_buffer,
                              std::uint8_t* upper_buffer,
                              size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    auto filtered = std::make_unique<std::uint8_t[]>(size);
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(current_buffer, i) -
                              floor((raw(current_buffer, i - bpp) + prior(upper_buffer, i)) / 2);
        filtered[i] = result;
    }
    for (size_t i {0}; i < size; ++i) {
        current_buffer[i] = filtered[i];
    }
}

void img::PNGImage::reverse_avg(std::uint8_t* current_buffer,
                                std::uint8_t* upper_buffer,
                                size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(current_buffer, i) +
                              floor((raw(current_buffer, i - bpp) + prior(upper_buffer, i)) / 2);
        current_buffer[i] = result;
    }
}

void img::PNGImage::apply_paeth(std::uint8_t* current_buffer,
                                std::uint8_t* upper_buffer,
                                size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    auto filtered = std::make_unique<std::uint8_t[]>(size);
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(current_buffer, i) -
                              paeth_predictor(raw(current_buffer, i - bpp),
                                              prior(upper_buffer, i),
                                              prior(upper_buffer, i - bpp));
        filtered[i] = result;
    }
    for (size_t i {0}; i < size; ++i) {
        current_buffer[i] = filtered[i];
    }
}

void img::PNGImage::reverse_paeth(std::uint8_t* current_buffer,
                                  std::uint8_t* upper_buffer,
                                  size_t size) {
    int bpp {std::max(sample_size * bit_depth / 8, 1)};
    for (int i {0}; i < size; ++i) {
        std::uint8_t result = raw(current_buffer, i) +
                              paeth_predictor(raw(current_buffer, i - bpp),
                                             prior(upper_buffer, i),
                                             prior(upper_buffer, i - bpp));
        current_buffer[i] = result;
    }
}
