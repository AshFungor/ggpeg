#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <cmath>
#include <iostream>

#define private public
#define protected public
#include <image/image.hpp>

TEST_CASE("Deflate compression test", "[added]") {
    using sc = img::Image::Scanline;
    std::uint8_t array_in[] = {0,
                             255, 153, 0,
                             255, 0, 0,
                             153, 255, 255,
                             153, 0, 255,
                             255, 0, 0,
                             255, 0, 0,
                             0, 153, 255,
                             255, 153, 0,
                             255, 255, 0, 0,
                             255, 255, 153,
                             0, 255, 0,
                             255, 153, 0,
                             255, 255, 0,
                             0, 255, 255,
                             153, 0, 255,
                             0, 153, 51,
                             255, 0, 255,
                             0, 0, 255,
                             255, 153, 0,
                             255, 0, 153,
                             51, 255, 255,
                            153, 0, 255};
    int size_in {sizeof(array_in)};
    int size_out {0};
    auto array_out = std::make_unique<char[]>(std::ceil(size_in * 1.1));
    sc::deflate(array_out.get(), size_out, reinterpret_cast<char*>(array_in), sizeof(array_in));
    std::cout << "size in: " << size_in << ", size out: " << size_out;
    for (int i {0}; i < size_out; i += 3) {
        std::cout << std::endl << array_out[i] << ' '
                  << array_out[i + 1] << ' ' << array_out[i + 2];
    }
}
