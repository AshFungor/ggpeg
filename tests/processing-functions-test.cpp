#include <catch2/catch_all.hpp>
#include <iostream>
#include <image/image.hpp>
#include <processing/processing.hpp>

TEST_CASE("test"){
    using img_t = img::PPMImage;
    img_t image{};
    image.read("resources/house_1.ppm");
    proc::negative(image);
    image.write("resources/negative.ppm");
}
