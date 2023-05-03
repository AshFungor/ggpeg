#include <catch2/catch_all.hpp>
#include <iostream>
#include <image/image.hpp>
#include <processing/processing.hpp>

TEST_CASE("test negative"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/house_1.ppm");
    proc::negative(image);
    image.write("resources/negative.ppm");
}

TEST_CASE("test crop"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west_2.ppm");
    proc::crop(image, 40, 0, 0, 0);
    image.write("resources/cropped.ppm");
}

TEST_CASE("test insert"){
    using img_t = img::PPMImage;
    img_t image;
    img_t other;
    image.read("resources/west_2.ppm");
    other.read("resources/tree_2.ppm");
    proc::insert(image, other, -100, 40);
    image.write("resources/inserted.ppm");
}

TEST_CASE("test reflex_x"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west_2.ppm");
    proc::reflex_x(image);
    image.write("resources/reflexed_x.ppm");
}

TEST_CASE("test reflex_y"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/west_2.ppm");
    proc::reflex_y(image);
    image.write("resources/reflexed_y.ppm");
}

TEST_CASE("test resize"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/stop_1.ppm");
    proc::resize(image, 0.1);
    image.write("resources/resized.ppm");
}

TEST_CASE("test rotate"){
    using img_t = img::PPMImage;
    img_t image;
    image.read("resources/sign_1.ppm");
    proc::rotate(image, -45);
    image.write("resources/rotated.ppm");
}
