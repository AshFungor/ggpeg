#include <catch2/catch_all.hpp>
#include <image/image.hpp>
#include <functional>
#include <algorithm>

// Not too big, not too small.
constexpr int magic_size = 12;

bool check_map_size(img::PixelMap& map, int exp_rows, int exp_columns) {
    return map.columns() == exp_columns && map.rows() == exp_rows;
}

void for_each_in_map(img::PixelMap& map, std::function<void(int row, int column)> action) {
    for (int row {0}; row < map.rows(); ++row) {
        for (int column {0}; column < map.columns(); ++column) {
            action(row, column);
        }
    }
}

TEST_CASE("Construction of class <PixelMap> and elements' access", "[base]") {
    img::PixelMap map {magic_size, magic_size};
    map.at(1, 1) = img::Color{0, 0, 0};
    REQUIRE(map.at(1, 1) == img::Color{0, 0, 0});
}

TEST_CASE("Expansion of class <PixelMap>", "[base]") {
    img::PixelMap map {magic_size, magic_size};
    for_each_in_map(map, [&map](int r, int c) { map.at(r, c) = img::Color{0, 0, 0}; });
    SECTION("One-side expansion") {
        using img::Side;
        map.expand(Side::right, 1);
        map.expand(Side::bottom, 1);
        REQUIRE(check_map_size(map, magic_size + 1, magic_size + 1));
        for_each_in_map(map, [&map](int row, int column) {
            if (column < magic_size && row < magic_size) {
                REQUIRE(map.at(row, column) == img::Color(0, 0, 0));
            } else {
                REQUIRE(map.at(row, column) == img::Color());
            }
        });
        map.expand(Side::left, 1);
        map.expand(Side::top, 1);
        REQUIRE(check_map_size(map, magic_size + 2, magic_size + 2));
        for_each_in_map(map, [&map](int row, int column) {
            img::Color& el = map.at(row, column);
            if ((0 < column && column < magic_size + 1) && (0 < row && row < magic_size + 1)) {
                REQUIRE(el== img::Color(0, 0, 0));
            } else {
                REQUIRE(el == img::Color());
            }
        });
    }
    SECTION("Double-side expansion") {
        using img::JointSide;
        map.expand(JointSide::left_and_right, 1, 1);
        REQUIRE(check_map_size(map, magic_size, magic_size + 2));
        for_each_in_map(map, [&map](int row, int column) {
            img::Color& el = map.at(row, column);
            if (0 < column && column < magic_size + 1) {
                REQUIRE(el== img::Color(0, 0, 0));
            } else {
                REQUIRE(el == img::Color());
            }
        });
        map.expand(JointSide::bottom_and_top, 1, 1);
        REQUIRE(check_map_size(map, magic_size + 2, magic_size + 2));
        for_each_in_map(map, [&map](int row, int column) {
            img::Color& el = map.at(row, column);
            if ((0 < column && column < magic_size + 1) && (0 < row && row < magic_size + 1)) {
                REQUIRE(el== img::Color(0, 0, 0));
            } else {
                REQUIRE(el == img::Color());
            }
        });
    }
}

TEST_CASE("Trimming class <PixelMap>", "[base]") {
    img::PixelMap map {magic_size, magic_size};
    SECTION("Trimming one-side") {
        using img::Side;
        map.trim(Side::right, 1);
        map.trim(Side::bottom, 1);
        map.trim(Side::left, 1);
        map.trim(Side::top, 1);
        REQUIRE(check_map_size(map, magic_size - 2, magic_size - 2));
        for_each_in_map(map, [&map](int r, int c) {
            REQUIRE(map.at(r, c) == img::Color{});
        });
    }
    SECTION("Trimming double-side") {
        using img::JointSide;
        map.trim(JointSide::left_and_right, 1, 1);
        map.trim(JointSide::bottom_and_top, 1, 1);
        REQUIRE(check_map_size(map, magic_size - 2, magic_size - 2));
        for_each_in_map(map, [&map](int r, int c) {
            REQUIRE(map.at(r, c) == img::Color{});
        });
    }

}
