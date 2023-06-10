// std headers
#include <cstddef>
#include <list>
#include <algorithm>
#include <bitset>
#include <iostream>
#include <cmath>

#include "image.hpp"

using i = img::Image;

char* i::Scanline::buffer_start {nullptr};
char* i::Scanline::buffer_end   {nullptr};

uint32_t i::Scanline::adler32(std::uint8_t* data, size_t len) {
    uint32_t a {1}, b {0};
    size_t index;
    for (index = 0; index < len; ++index)
    {
        a = (a + data[index]) % mod_adler;
        b = (b + a) % mod_adler;
    }
    return (b << 16) | a;
}

std::uint8_t* i::Scanline::move_buffer(std::uint8_t* data, std::int32_t number) {
    if (number < 0) {
        return data - std::min<std::int32_t>(abs(number),
                                             data - reinterpret_cast<std::uint8_t*&>(buffer_start));
    }
    return data + std::min<std::int32_t>(number,
                                         reinterpret_cast<std::uint8_t*&>(buffer_end) - data);
}

i::Scanline::triplet i::Scanline::find_match(std::uint8_t* sliding_window, int size) {
    std::uint32_t i {0};
    while (sliding_window[i] != sliding_window[size] && i < size)
    { ++i; }
    // match not found
    if (i >= size) {
        return {sliding_window[size], 0, 0};
    }
    std::uint32_t offset {size - i};
    while (sliding_window + (i - (size - offset) + size) <
               reinterpret_cast<std::uint8_t*&>(buffer_end) &&
           sliding_window[i] == sliding_window[i - (size - offset) + size] &&
           i < size)
    { ++i; }
    std::uint32_t length {offset - (size - i)};
    if (length < 3) {
        return {sliding_window[size], 0, 0};
    }
    // buffer reached the end
    if (sliding_window + (i - (size - offset) + size) >=
        reinterpret_cast<std::uint8_t*&>(buffer_end)) {
        return {0, offset, length};
    }
    return {sliding_window[i - (size - offset) + size], offset, length};
}

std::list<i::Scanline::triplet> i::Scanline::lz77(std::uint8_t* data, int size,
                                                  const std::uint64_t window_size) {
    std::list<triplet> result {};
    std::uint32_t position {0};
    while (position < size) {
        auto node = find_match(move_buffer(data, -window_size),
                               data - move_buffer(data, -window_size));
        data += node.length + 1;
        position += node.length + 1;
        result.push_back(node);
    }
    return result;
}

void img::Image::Scanline::add_bits(std::bitset<block_size * 8>& source, std::uint32_t bits, int& pos, int number) {
    number -= 1;
    while (number >= 0) {
        source.set(pos, (bits >> number) & 1);
        ++pos;
        --number;
    }
}

std::uint32_t i::Scanline::match_offset(std::uint32_t offset, int& out_len) {
    int dist {1};
    if (offset <= 4) {
        return offset - dist;
    }
    std::uint32_t counter     {0};
    std::uint32_t curr_offset {4};
    std::uint32_t number      {3};
    while (curr_offset + dist <= offset) {
        curr_offset += dist;
        number += 1;
        if (!counter) {
            counter = 1;
            dist *= 2;
        } else {
            counter -= 1;
        }
    }
    std::uint32_t result {number};
//    std::cout << "inside: " << offset << ' ' << curr_offset << std::endl;
    result |= (offset - curr_offset) << 5;
    out_len = std::round(std::log2(dist));
    return result;
}

std::uint32_t i::Scanline::match_length(std::uint32_t length, int& out_len) {
    int dist {1};
    if (length == 258) {
        return 285;
    }
    if (length <= 10) {
        return length - dist + 257 - 2;
    }
    std::uint32_t counter     {0};
    std::uint32_t curr_length {10};
    std::uint32_t number      {264};
    while (curr_length + dist <= length) {
        curr_length += dist;
        number += 1;
        if (!counter) {
            counter = 3;
            dist *= 2;
        } else {
            counter -= 1;
        }
    }
    std::uint32_t result {number};
//    std::cout << "inside: " << length << ' ' << curr_length << std::endl;
    result |= (length - curr_length) << 9;
    out_len = std::round(std::log2(dist));
    return result;
}

int img::Image::Scanline::deflate(char* dest, int& size_out, char* const data, int size) {
    buffer_start = data;
    buffer_end = data + size;
    dest[0] = 0b01111000;
    dest[1] = 0b11011010;
    std::vector<std::list<triplet>> blocks {};
    std::uint32_t curr_offset {0};
    while (curr_offset < size) {
        auto curr_data = data + curr_offset;
        auto optimized = lz77(reinterpret_cast<std::uint8_t*&>(curr_data),
                              std::min<std::uint32_t>(size - curr_offset, block_size),
                              window_size);
        blocks.push_back(optimized);
        curr_offset += std::min<std::uint32_t>(size - curr_offset, block_size);
    }
//    for (int i {1}; i < 200; i += 20) {
//        auto res = match_offset(i);
//        std::cout << i << ' ' << (res >> 5) << ' ' << (res & 0b11111) << std::endl;
//    }
//    return 0;
//    for (int i {3}; i < 30; i += 1) {
//        auto res = match_length(i);
//        std::cout << i << ' ' << (res >> 9) << ' ' << (res & 0b111111111) << std::endl;
//    }
//    return 0;
    int position_byte {2};
    int position_bit  {0};
    int index         {1};
    // encode with Huffman
    for (auto& list : blocks) {
        std::bitset<block_size * 8> bits;
        if (index++ == blocks.size()) {
            bits.set(position_bit++);
            bits.set(position_bit++);
            ++position_bit;
        } else {
            ++position_bit;
            bits.set(position_bit++);
            ++position_bit;
        }
        int local_index {1};
        for (auto& triple : list) {
            std::uint64_t encoded_value = {0};
            // case (length + offset)
            if (triple.length != 0) {
                int offset_len, length_len;
                auto length = match_length(triple.length, length_len);
                auto offset = match_offset(triple.offset, offset_len);
                if (triple.length < 115) {
                    add_bits(bits, length & 0b111111111 - 256, position_bit, 7);
                } else if (triple.length >= 115) {
                    add_bits(bits, length & 0b111111111 - 280 + 0b11000000, position_bit, 8);
                }
                length >>= 9;
                add_bits(bits, length, position_bit, length_len - 9);
                add_bits(bits, offset & 0b11111, position_bit, 5);
                add_bits(bits, offset >> 5, position_bit, offset_len - 5);
                if (local_index++ == list.size()) {
                    break;
                }
            }
            // case (literal)
            if (triple.byte < 144) {
                add_bits(bits, triple.byte + 0b00110000, position_bit, 8);
            } else if (143 < triple.byte) {
                add_bits(bits, triple.byte + 0b110010000 - 144, position_bit, 9);
            }
        }
        position_bit += 8;
        // inverse order of all bits in bytes (docs say so) and write to dest
        int i;
        for (i = 0; i < position_bit; i += 8) {
            std::uint8_t byte {0};
            for (int p {0}; p < 8; ++p) {
                byte += bits[i + p] << p;
            }
            dest[position_byte++] = reinterpret_cast<char&>(byte);
        }
        // fill space before the next byte
        position_bit += (position_bit - i);
    }

    auto check_sum = adler32(reinterpret_cast<std::uint8_t* const&>(data), size_out);
    std::uint8_t byte = check_sum & 0xFF;
    dest[position_byte++] = reinterpret_cast<std::uint8_t&>(byte);
    byte = (check_sum & 0xFF00) >> 8;
    dest[position_byte++] = reinterpret_cast<std::uint8_t&>(byte);
    byte = (check_sum & 0xFF0000) >> 16;
    dest[position_byte++] = reinterpret_cast<std::uint8_t&>(byte);
    byte = (check_sum & 0xFF000000) >> 24;
    dest[position_byte++] = reinterpret_cast<std::uint8_t&>(byte);

    size_out = position_byte;

    return 0;
}
