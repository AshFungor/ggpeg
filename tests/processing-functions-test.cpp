#include <catch2/catch_all.hpp>
#include <iostream>
#include <image/image.hpp>
#include <processing/processing.hpp>

TEST_CASE("test"){
    std::cout << "1"; 
    using img_t = img::PPMImage;
    img_t image;
    image.read("resoures/house_1.ppm");


}