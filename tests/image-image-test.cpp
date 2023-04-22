#include <catch2/catch_all.hpp>
#include <image/image.hpp>


TEST_CASE("PPM reading an writing in <Image> class", "[base]") {
    img::Image image {};
    image.read_ppm("resources/west_1.ppm");
    REQUIRE(image.is_loaded());
    image.write_out_ppm("resources/test_output.ppm");
    img::Image result {};
    result.read_ppm("resources/test_output.ppm");
    REQUIRE(result.is_loaded());
    img::PixelMap res_map = result.get_pixel_map();
    img::PixelMap initial_map = image.get_pixel_map();
    REQUIRE(res_map.columns() == initial_map.columns());
    REQUIRE(res_map.rows() == initial_map.rows());

    bool check {1};
    for (int row {0}; row < res_map.rows(); ++row) {
        for (int column {0}; column < res_map.columns(); ++column) {
            check &= res_map.at(row, column) == initial_map.at(row, column);
        }
    }
    REQUIRE(check);
}
