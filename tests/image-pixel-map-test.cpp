#include <catch2/catch_all.hpp>
#include <image/image.hpp>
#include <functional>
#include <algorithm>

TEST_CASE("Construction of class <PixelMap> and elements access", "[pixel_map_base]") {
    img::PixelMap map {12, 12};
    map.at(1, 1) = img::Color{1, 1, 1};
    REQUIRE(map.at(1, 1) == img::Color{1, 1, 1});
    map.at(2, 2) = img::Color{2, 2, 2};
    REQUIRE(map.at(2, 2) == img::Color{2, 2, 2});
}

TEST_CASE("Expansion of class <PixelMap>", "[pixel_map_expansion]") {
    img::PixelMap map {12, 12};
    for (int row {0}; row < map.rows(); ++row) {
        for (int column {0}; column < map.columns(); ++column) {
            map.at(row, column) = img::Color(0, 0, 0);
        }
    }
    SECTION("One-side expansion") {
        using img::Side;
        map.expand(Side::right, 2);
        map.expand(Side::bottom, 2);
        REQUIRE(map.columns() == 14);
        REQUIRE(map.rows() == 14);
        for (int row {0}; row < map.rows(); ++row) {
            for (int column {0}; column < map.columns(); ++column) {
                if (column < 12 && row < 12) {
                    REQUIRE(map.at(row, column) == img::Color(0, 0, 0));
                } else {
                    REQUIRE(map.at(row, column) == img::Color());
                }
            }
        }
        map.expand(Side::left, 2);
        map.expand(Side::top, 2);
        REQUIRE(map.columns() == 16);
        REQUIRE(map.rows() == 16);
        for (int row {0}; row < map.rows(); ++row) {
            for (int column {0}; column < map.columns(); ++column) {
                img::Color& el = map.at(row, column);
                if ((1 < column && column < 14) && (1 < row && row < 14)) {
                    CAPTURE(el.R(), el.G(), el.B(), row, column);
                    REQUIRE(el== img::Color(0, 0, 0));
                } else {
                    CAPTURE(el.R(), el.G(), el.B(), row, column);
                    REQUIRE(el == img::Color());
                }
            }
        }
    }
    SECTION("Double-side expansion") {
        using img::JointSide;
        map.expand(JointSide::left_and_right, 2, 2);
        REQUIRE(map.columns() == 16);
        REQUIRE(map.rows() == 12);
        for (int row {0}; row < map.rows(); ++row) {
            for (int column {0}; column < map.columns(); ++column) {
                img::Color& el = map.at(row, column);
                if (1 < column && column < 14) {
                    CAPTURE(el.R(), el.G(), el.B(), row, column);
                    REQUIRE(el== img::Color(0, 0, 0));
                } else {
                    CAPTURE(el.R(), el.G(), el.B(), row, column);
                    REQUIRE(el == img::Color());
                }
            }
        }
        map.expand(JointSide::bottom_and_top, 2, 2);
        REQUIRE(map.columns() == 16);
        REQUIRE(map.rows() == 16);
        for (int row {0}; row < map.rows(); ++row) {
            for (int column {0}; column < map.columns(); ++column) {
                img::Color& el = map.at(row, column);
                if ((1 < column && column < 14) && (1 < row && row < 14)) {
                    CAPTURE(el.R(), el.G(), el.B(), row, column);
                    REQUIRE(el== img::Color(0, 0, 0));
                } else {
                    CAPTURE(el.R(), el.G(), el.B(), row, column);
                    REQUIRE(el == img::Color());
                }
            }
        }
    }
}

TEST_CASE("Large expansion of class <PixelMap>", "[pixel_map_expansion_complexity]") {
    img::PixelMap map {0, 0};
    map.expand(img::JointSide::bottom_and_top, 1'000, 1'000);
    map.expand(img::JointSide::left_and_right, 1'000, 1'000);
    REQUIRE(map.columns() == 2'000);
    REQUIRE(map.rows() == 2'000);
    for (int row {0}; row < map.rows(); ++row) {
        for (int column {0}; column < map.columns(); ++column) {
            REQUIRE(map.at(row, column) == img::Color());
        }
    }
}
