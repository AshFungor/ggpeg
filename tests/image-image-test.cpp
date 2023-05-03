// std
#include <catch2/catch_all.hpp>
#include <string>
#include <memory>

#define private public
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

}

TEST_CASE("PNG helper functions", "[base]") {
    // compare_chunks test
    char byte_array[4] {'H', 'I', 'H', 'I'};
    char byte_array_eq[4] {'H', 'I', 'H', 'I'};
    char byte_array_diff[4] {'H', 'I', 'H', 'O'};
    char byte_array_long[5] {'H', 'I', 'H', 'I', 'I'};
    bool check = true;
    check &= img::PNGImage::_cmp_chunks(byte_array, 4, byte_array_eq, 4);
    check &= !img::PNGImage::_cmp_chunks(byte_array, 4, byte_array_diff, 4);
    check &= img::PNGImage::_cmp_chunks(byte_array_eq, 4, byte_array, 4);
    check &= !img::PNGImage::_cmp_chunks(byte_array_diff, 4, byte_array, 4);
    check &= !img::PNGImage::_cmp_chunks(byte_array_diff, 4, byte_array_long, 5);
    REQUIRE(check);
    // parse_chunk test
    char number_255[2] {0, ~0};
    REQUIRE(img::PNGImage::_parse_chunk(number_255, 2) == 255);
    char number_255_0[2] {~0, 0};
    REQUIRE(img::PNGImage::_parse_chunk(number_255_0, 2) == 255 << 8);
    char number_255_255_255[3] {~0, ~0, ~0};
    REQUIRE(img::PNGImage::_parse_chunk(number_255_255_255, 3) == (255 << 16) + (255 << 8) + 255);
    // extract_chunk test
    auto bytes = std::make_unique<char[]>(10);
    for (int i {0}; i < 10; ++i) { bytes[i] = i; }
    char chunk_1b[1], chunk_2b[2], chunk_3b[3], chunk_4b[4];
    char c_chunk_1b[1]  {0},
         c_chunk_2b[2]  {1, 2},
         c_chunk_3b[3]  {3, 4, 5},
         c_chunk_4b[4]  {6, 7, 8, 9};
    auto buffer = bytes.get();
    img::PNGImage::_extr_chunk(buffer, chunk_1b, 1);
    REQUIRE(img::PNGImage::_cmp_chunks(chunk_1b, 1, c_chunk_1b, 1));
    img::PNGImage::_extr_chunk(buffer, chunk_2b, 2);
    REQUIRE(img::PNGImage::_cmp_chunks(chunk_2b, 2, c_chunk_2b, 2));
    img::PNGImage::_extr_chunk(buffer, chunk_3b, 3);
    REQUIRE(img::PNGImage::_cmp_chunks(chunk_3b, 3, c_chunk_3b, 3));
    img::PNGImage::_extr_chunk(buffer, chunk_4b, 4);
    REQUIRE(img::PNGImage::_cmp_chunks(chunk_4b, 4, c_chunk_4b, 4));
}

