// std
#include <catch2/catch_all.hpp>
#include <string>
#include <memory>

#define private public
#define protected public
#include <image/image.hpp>

TEST_CASE("PPM reading an writing in <Image> class", "[base]") {
    using img_t = img::PPMImage;
    const std::string files[] {
        "boxes_1.ppm",  "house_1.ppm",
        "moreboxes_1.ppm",  "sign_1.ppm",
        "stop_1.ppm", "stop_2.ppm",
        "tree_1.ppm",  "west_1.ppm",
        "boxes_2.ppm",  "house_2.ppm",
        "moreboxes_2.ppm",  "sign_2.ppm",
        "tree_2.ppm",  "west_2.ppm",
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

//    img::PPMImage img {};
//    std::string_view path {"resources/house_2.ppm"};
//    img.read(path);
//    // img.read(path);
//    img::PNGImage img_2 {};
//    img_2.get_map() = img.get_map();
//    img_2.write("resources/result.png");

    img::PNGImage img {};
    std::string_view path {"resources/result.png"};
    img.read(path);
    // img.read(path);
    img::PPMImage img_2 {};
    img_2.get_map() = img.get_map();
    img_2.write("resources/result_2.ppm");

}


