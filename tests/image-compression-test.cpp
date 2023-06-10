#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <memory>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

#define private public
#define protected public
#include <image/image.hpp>

using sc = img::Image::Scanline;

bool check_triple(sc::triplet one, sc::triplet other) {
    return (one.byte == other.byte) && (one.length == other.length) && (one.offset == other.offset);
}

TEST_CASE("LZ77", "[added]") {
    char buffer[] {"abacabadaba"};
    sc::buffer_start = buffer;
    sc::buffer_end = buffer + sizeof(buffer);
    auto res = sc::lz77(reinterpret_cast<std::uint8_t*>(buffer), sizeof(buffer), sc::window_size);
    // Since it is required that length of match must be at least 3, result should be:
    // (a, 0, 0) - just symbol a
    // (b, 0, 0) - just symbol b
    // (a, 0, 0) - just symbol a (does not satisfy the condition)
    // (c, 0, 0) - just symbol c
    // (d, 3, 4) - return by 4 and copy 3 symbols + symbol d
    // (0, 3, 8) - return by 8 and copy 3 symbols + nothing (special code)
    bool check {true};
    auto iter = res.begin();
    check &= check_triple(*(iter++), {'a', 0, 0});
    check &= check_triple(*(iter++), {'b', 0, 0});
    check &= check_triple(*(iter++), {'a', 0, 0});
    check &= check_triple(*(iter++), {'c', 0, 0});
    check &= check_triple(*(iter++), {'d', 4, 3});
    check &= check_triple(*(iter++), {0, 8, 3});
    REQUIRE(check);
}

TEST_CASE("Appending bits to the end of bitset", "[added]") {
    std::bitset<sc::block_size * 8> bits {};
    int pos {0};
    sc::add_bits(bits, 0xF, pos, 4);
    sc::add_bits(bits, 0xF0, pos, 8);
    bool check {true};
    CHECK(pos == 12);
    for (int i {0}; i < pos - 4; ++i) {
        check &= bits[i];
    }
    for (int i {pos - 4}; i < pos; ++i) {
        check &= !bits[i];
    }
    sc::add_bits(bits, 0xF, pos, 4);
    CHECK(pos == 16);
    for (int i {pos - 4}; i < pos; ++i) {
        check &= bits[i];
    }
    REQUIRE(check);
}

TEST_CASE("Matching length to static Huffman code", "[added]") {
    int initial_length {20}, bit_len;
    CHECK((sc::match_length(initial_length, bit_len) & 0x1FF) == 269);
    CHECK(bit_len == 2);
    CHECK((sc::match_length(initial_length, bit_len) >> 9) == 1);
    initial_length = 200;
    CHECK((sc::match_length(initial_length, bit_len) & 0x1FF) == 283);
    CHECK(bit_len == 5);
    CHECK((sc::match_length(initial_length, bit_len) >> 9) == 5);
}

TEST_CASE("Matching offset to static Huffman code", "[added]") {
    int initial_offset {11}, bit_len;
    CHECK((sc::match_offset(initial_offset, bit_len) & 0x1F) == 6);
    CHECK(bit_len == 2);
    CHECK((sc::match_offset(initial_offset, bit_len) >> 5) == 2);
    initial_offset = 2060;
    CHECK((sc::match_offset(initial_offset, bit_len) & 0x1F) == 22);
    CHECK(bit_len == 10);
    CHECK((sc::match_offset(initial_offset, bit_len) >> 5) == 11);
}

TEST_CASE("Deflate compression", "[added]") {
    std::uint8_t array_in[] =   {0,
                                255, 153, 0,
                                255, 0, 0,
                                153, 255, 255,
                                153, 0, 255,
                                255, 0, 0,
                                255, 0, 0,
                                0, 153, 255,
                                255, 153, 0,
                                255, 255, 0, 0,
                                255, 255, 153,
                                0, 255, 0,
                                255, 153, 0,
                                255, 255, 0,
                                0, 255, 255,
                                153, 0, 255,
                                0, 153, 51,
                                255, 0, 255,
                                0, 0, 255,
                                255, 153, 0,
                                255, 0, 153,
                                51, 255, 255,
                                153, 0, 255};
    int size_in {sizeof(array_in)};
    int size_out {0};
    auto array_out = std::make_unique<char[]>(std::ceil(size_in * 1.1));
    sc::deflate(array_out.get(), size_out, reinterpret_cast<char*>(array_in), sizeof(array_in));
    std::fstream file;
    file.open("output.txt", std::ios::binary | std::ios::out);
    for (int i{0}; i < size_out; ++i) {
        file << array_out[i];
    }
    file.close();
//    std::cout << "size in: " << size_in << ", size out: " << size_out;
//    std::cout << std::hex;
//    for (int i {0}; i < size_out; i += 3) {
//        std::cout << std::endl << int(array_out[i]) << '\t'
//                  << int(array_out[i + 1]) << '\t' << int(array_out[i + 2]);
//    }
}
