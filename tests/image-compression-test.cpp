#include <catch2/catch_test_macros.hpp>

#define private public
#define protected public
#include <image/image.hpp>

TEST_CASE("Deflate compression test", "[added]") {
    using sc = img::Image::Scanline;
    char array[] = "abacabacabadaca";
    int size_out {0};
    char array_2[10];
    sc::deflate(array_2, size_out, array, sizeof(array));
}
