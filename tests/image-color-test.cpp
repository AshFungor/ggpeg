#include <catch2/catch_test_macros.hpp>
#include <image/image.hpp>

bool colors_match(int r, int g, int b, img::Color color) {
    bool result {1};
    result &= color.R() == r;
    result &= color.G() == g;
    result &= color.B() == b;
    return result;
}

TEST_CASE("Construction of <Color> class and basic usage", "[color_base]") {
    // Base constructor must initialize white color.
    img::Color test_color{};
    REQUIRE(colors_match(255, 255, 255, test_color));
    // Constructor with three colors.
    test_color = img::Color(12, 12, 12);
    REQUIRE(colors_match(12, 12, 12, test_color));
    // Change each color separately
    test_color.R(24);
    test_color.G(24);
    test_color.B(24);
    REQUIRE(colors_match(24, 24, 24, test_color));
}

