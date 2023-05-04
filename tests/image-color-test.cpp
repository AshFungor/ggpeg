#include <catch2/catch_test_macros.hpp>
#include <image/image.hpp>
#include <random>
#include <cstddef>

bool colors_match(int r, int g, int b, img::Color color) {
    bool result {1};
    result &= color.R() == r;
    result &= color.G() == g;
    result &= color.B() == b;
    return result;
}

TEST_CASE("Construction of <Color> class and basic usage", "[base]") {
    // Base constructor must initialize white color.
    img::Color test_color{};
    REQUIRE(colors_match(0, 0, 0, test_color));
    // Constructor with three colors.
    std::mt19937 rd { std::random_device{}() };
    for (int times {0}; times < 20; ++times) {
        std::uint8_t red = rd() % 256;
        std::uint8_t green = rd() % 256;
        std::uint8_t blue = rd() % 256;
        test_color = img::Color(red, green, blue);
        REQUIRE(colors_match(red, green, blue, test_color));
        REQUIRE(img::Color{red, green, blue} == test_color);
        REQUIRE_FALSE(img::Color{(red + 1) % 256, green, blue} == test_color);
        REQUIRE_FALSE(img::Color{red, (green + 1) % 256, blue} == test_color);
        REQUIRE_FALSE(img::Color{red, green, (blue + 1) % 256} == test_color);
        REQUIRE_FALSE(img::Color{(red + 1) % 256, (green + 1) % 256, (blue + 1) % 256}
                      == test_color);
        red = rd() % 256;
        green = rd() % 256;
        blue = rd() % 256;
        // Change each color separately
        test_color.R(red);
        test_color.G(green);
        test_color.B(blue);
        REQUIRE(colors_match(red, green, blue, test_color));
    }

}

