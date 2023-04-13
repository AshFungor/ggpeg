#include <catch2/catch_test_macros.hpp>
#include <image/image.hpp>

TEST_CASE("Construction of <Color> class and basic usage") {
    // Base constructor must initialize white color.
    img::Color test_color{};
    REQUIRE(test_color.R() == 255);
    REQUIRE(test_color.G() == 255);
    REQUIRE(test_color.B() == 255);
}

