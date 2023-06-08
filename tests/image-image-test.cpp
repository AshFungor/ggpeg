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
        "simple.png",
        "hut.png",
        "clouds.png",
        "bumblebee.png"
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
