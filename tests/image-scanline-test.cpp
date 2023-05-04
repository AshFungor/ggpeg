#include <catch2/catch_all.hpp>
#include <memory>
#include <string>

#define private public
#define protected public

#include <image/image.hpp>

TEST_CASE("Scanline functionality", "[base]") {
    using Scanline = img::Image::Scanline;
    char data[] {"Hello, world!\n"};
    SECTION("writing functions") {
        Scanline file {"resources/test_text_file.txt", img::Image::ScanMode::write};
        file.expand_buffer(15);
        REQUIRE(file.size() == 15);
        auto data_ptr = std::make_unique<char[]>(sizeof(data));
        for (int i {0}; i < sizeof(data); ++i) {data_ptr[i] = data[i]; }
        file.set_chunk(0, 14, std::move(data_ptr));
        file.call_write(15);
        REQUIRE(file.size() == 0);
    }
    SECTION("reading functions") {
        Scanline file {"resources/test_text_file.txt", img::Image::ScanMode::read};
        file.call_read(15);
        REQUIRE(file.size() == 15);
        auto read_data = file.get_chunk(0, 15);
        REQUIRE(Scanline::_cmp_chunks(read_data.get(), 15, data, 15));
    }
}


TEST_CASE("Scanline helper functions", "[base]") {
    // compare_chunks test
    char byte_array[4] {'H', 'I', 'H', 'I'};
    char byte_array_eq[4] {'H', 'I', 'H', 'I'};
    char byte_array_diff[4] {'H', 'I', 'H', 'O'};
    char byte_array_long[5] {'H', 'I', 'H', 'I', 'I'};
    bool check = true;
    check &= img::Image::Scanline::_cmp_chunks(byte_array, 4, byte_array_eq, 4);
    check &= !img::Image::Scanline::_cmp_chunks(byte_array, 4, byte_array_diff, 4);
    check &= img::Image::Scanline::_cmp_chunks(byte_array_eq, 4, byte_array, 4);
    check &= !img::Image::Scanline::_cmp_chunks(byte_array_diff, 4, byte_array, 4);
    check &= !img::Image::Scanline::_cmp_chunks(byte_array_diff, 4, byte_array_long, 5);
    REQUIRE(check);
    // parse_chunk test
    char number_255[2] {0, ~0};
    REQUIRE(img::Image::Scanline::_parse_chunk(number_255, 2)
            == 255);
    char number_255_0[2] {~0, 0};
    REQUIRE(img::Image::Scanline::_parse_chunk(number_255_0, 2)
            == 255 << 8);
    char number_255_255_255[3] {~0, ~0, ~0};
    REQUIRE(img::Image::Scanline::_parse_chunk(number_255_255_255, 3)
            == (255 << 16) + (255 << 8) + 255);
    // extract_chunk test
    auto bytes = std::make_unique<char[]>(10);
    for (int i {0}; i < 10; ++i) { bytes[i] = i; }
    char chunk_1b[1], chunk_2b[2], chunk_3b[3], chunk_4b[4];
    char c_chunk_1b[1]  {0},
         c_chunk_2b[2]  {1, 2},
         c_chunk_3b[3]  {3, 4, 5},
         c_chunk_4b[4]  {6, 7, 8, 9};
    auto buffer = bytes.get();
    img::Image::Scanline::_extr_chunk(&buffer, chunk_1b, 1);
    REQUIRE(img::Image::Scanline::_cmp_chunks(chunk_1b, 1, c_chunk_1b, 1));
    img::Image::Scanline::_extr_chunk(&buffer, chunk_2b, 2);
    REQUIRE(img::Image::Scanline::_cmp_chunks(chunk_2b, 2, c_chunk_2b, 2));
    img::Image::Scanline::_extr_chunk(&buffer, chunk_3b, 3);
    REQUIRE(img::Image::Scanline::_cmp_chunks(chunk_3b, 3, c_chunk_3b, 3));
    img::Image::Scanline::_extr_chunk(&buffer, chunk_4b, 4);
    REQUIRE(img::Image::Scanline::_cmp_chunks(chunk_4b, 4, c_chunk_4b, 4));
    // CRC test
    char sample_input[] = { 0x49,0x48,0x44,0x52,0x00,0x00,0x01,0x77,0x00,
                         0x00,0x01,0x68,0x08,0x06,0x00,0x00,0x00 };
    char iend_input[] = {'I', 'E', 'N', 'D'};
    REQUIRE(img::Image::Scanline::_crc(sample_input, 17) == 2889923504);
    REQUIRE(img::Image::Scanline::_crc(iend_input, 4) == 2923585666);

}
