// std
#include <catch2/catch_all.hpp>
#include <string>
#include <memory>
#include <processing/processing.hpp>

#define private public
#define protected public
#include <image/image.hpp>

TEST_CASE("PPM reading and writing in <Image> class", "[base]") {
    using img_t = img::PPMImage;
    const std::string files[] {
        "boxes.ppm",
        "house.ppm",
        "stop.ppm",
        "sign.ppm",
        "tree.ppm",
        "west.ppm",
    };


    for (auto& img_path : files) {
        auto image_1 = std::make_unique<img_t>(img_t{});
        auto image_2 = std::make_unique<img_t>(img_t{});
        image_1->read(std::string{"resources/"}.append(img_path));
        REQUIRE(image_1->good());
        image_1->write(std::string{"resources/"}.append("result.ppm"));
        image_2->read(std::string{"resources/"}.append("result.ppm"));
        REQUIRE((image_1->good() && image_2->good()));
        img::PixelMap& img_1_map {image_1->get_map()}, img_2_map {image_2->get_map()};
        REQUIRE(img_1_map.rows() == img_2_map.rows());
        REQUIRE(img_1_map.columns() == img_2_map.columns());
        bool check {1};
        for (int row {0}; row < img_1_map.rows(); ++row) {
            for (int column {0}; column < img_1_map.columns(); ++column) {
                check &= img_1_map.at(row, column) == img_2_map.at(row, column);
            }
        }
        REQUIRE(check);
    }
}

TEST_CASE("PNG reading and writing in <Image> class", "[base]") {
    using img_t = img::PNGImage;
    const std::string files[] {
//        "simple.png",
//        "hut.png",
        "clouds.png",
        "bumblebee.png"
    };

    for (auto& img_path : files) {
        auto image_1 = std::make_unique<img_t>(img_t{});
        auto image_2 = std::make_unique<img_t>(img_t{});
        image_1->read(std::string{"resources/"}.append(img_path));
        REQUIRE(image_1->good());
        image_1->write(std::string{"resources/"}.append("result.png"));
        image_2->read(std::string{"resources/"}.append("result.png"));
        REQUIRE((image_1->good() && image_2->good()));
        img::PixelMap& img_1_map {image_1->get_map()}, img_2_map {image_2->get_map()};
        REQUIRE(img_1_map.rows() == img_2_map.rows());
        REQUIRE(img_1_map.columns() == img_2_map.columns());
        bool check {1};
        for (int row {0}; row < img_1_map.rows(); ++row) {
            for (int column {0}; column < img_1_map.columns(); ++column) {
                check &= img_1_map.at(row, column) == img_2_map.at(row, column);
            }
        }
        REQUIRE(check);
    }
}

TEST_CASE("PNG exceptions", "[added]") {
    using img_t = img::PNGImage;
    const std::string bad_files_ihdr[] {
        "bad_ihdr_bit_depth.png",
        "bad_ihdr_methods.png",
        "bad_ihdr_size.png"
    };

    auto image = img_t{};
    for (auto& file : bad_files_ihdr) {
        CHECK_THROWS_AS(image.read(std::string{"resources/"}.append(file)),
                        img_t::IHDRDecoderError);
    }
}

TEST_CASE("Get type of image", "[added]") {
    std::string image_png {"simple.png"};
    std::string image_ppm {"boxes.ppm"};
    REQUIRE(img::get_type(std::string{"resources/"}.append(image_png)) == img::ImageType::PNG);
    REQUIRE(img::get_type(std::string{"resources/"}.append(image_ppm)) == img::ImageType::PPM);
}

TEST_CASE("Convert image to new type", "[added]") {
    img::PNGImage img_png {};
    img_png.read("resources/simple.png");
    auto new_ppm = img::convert(img_png, img::ImageType::PPM);
    new_ppm.write("resources/result.ppm");
    REQUIRE(img::get_type("resources/result.ppm") == img::ImageType::PPM);
    auto new_png = img::convert(new_ppm, img::ImageType::PNG);
    new_png.write("resources/result.png");
    REQUIRE(img::get_type("resources/result.png") == img::ImageType::PNG);
}
