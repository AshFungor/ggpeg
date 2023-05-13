#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <cstddef>

#define private public
#define protected public
#define BYTE std::uint8_t

#include <image/image.hpp>

void fill_buffer(BYTE start, BYTE* buffer, size_t size) {
    size_t index {0};
    while (size--) {
        buffer[index++] = start;
        start += 2;
    }
}

TEST_CASE("<sub>, <up> and <avg> filters", "[base]") {
    using img = img::PNGImage;
    img test_img;
    // Presets.
    test_img.bit_depth = 8;                 // 1 byte per sample unit
    test_img.sample_size = 1;               // 1 byte per sample
    constexpr size_t magic_size {10};       // size of buffers for this test
    // Test section.
    auto buffer_1 = std::make_unique<BYTE>(magic_size);
    fill_buffer(0, buffer_1.get(), magic_size);
    auto buffer_2 = std::make_unique<BYTE>(magic_size);
    fill_buffer(0, buffer_2.get(), magic_size);
    auto initial_buffer = std::make_unique<BYTE>(magic_size);
    fill_buffer(0, initial_buffer.get(), magic_size);
    // Sub filter.
    test_img.apply_sub(buffer_1.get(), magic_size);
    test_img.reverse_sub(buffer_1.get(), magic_size);
    auto ptr_buff_1 = buffer_1.get();
    auto ptr_init_buff = initial_buffer.get();
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
    // Up filter.
    // For first scanline.
    test_img.apply_up(buffer_1.get(), nullptr, magic_size);
    test_img.reverse_up(buffer_1.get(), nullptr, magic_size);
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
    // For defined upper buffer (any other scanline except for the upper one)
    test_img.apply_up(buffer_1.get(), buffer_2.get(), magic_size);
    test_img.reverse_up(buffer_1.get(), buffer_2.get(), magic_size);
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
    // Average filter.
    // For first scanline.
    test_img.apply_avg(buffer_1.get(), nullptr, magic_size);
    test_img.reverse_avg(buffer_1.get(), nullptr, magic_size);
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
    // For any other.
    test_img.apply_avg(buffer_1.get(), buffer_2.get(), magic_size);
    test_img.reverse_avg(buffer_1.get(), buffer_2.get(), magic_size);
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
    // Paeth filter.
    // For first scanline.
    test_img.apply_paeth(buffer_1.get(), nullptr, magic_size);
    test_img.reverse_paeth(buffer_1.get(), nullptr, magic_size);
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
    // For any other.
    test_img.apply_paeth(buffer_1.get(), buffer_2.get(), magic_size);
    test_img.reverse_paeth(buffer_1.get(), buffer_2.get(), magic_size);
    REQUIRE(img::Image::Scanline::_cmp_chunks(reinterpret_cast<char*&>(ptr_buff_1), magic_size,
                                              reinterpret_cast<char*&>(ptr_init_buff), magic_size));
}

