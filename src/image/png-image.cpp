#include "image.hpp"

// compression
#include <zlib.h>

// std headers
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>

char img::PNGImage::_chunk_1b[1] {};
char img::PNGImage::_chunk_4b[4] {};
char img::PNGImage::_chunk_8b[8] {};


void img::PNGImage::read_chunk_header(char*& buffer,
                                      img::PNGImage::Chunk& chunk,
                                      size_t& size) {
    // first 4-byte chunk is length
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    size = Scanline::_parse_chunk(_chunk_4b, 4);
    // second 4-byte chunk is name
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    if (Scanline::_cmp_chunks(_chunk_4b, 4, _ihdr_name, 4)) {
        chunk = img::PNGImage::Chunk::IHDR;
    } else if (Scanline::_cmp_chunks(_chunk_4b, 4, _idat_name, 4)) {
        chunk = img::PNGImage::Chunk::IDAT;
    } else if (Scanline::_cmp_chunks(_chunk_4b, 4, _iend_name, 4)) {
        chunk = img::PNGImage::Chunk::IEND;
    }
}



bool img::PNGImage::read_crc(char* buffer, size_t size) {
    auto expected_crc = Scanline::_crc(buffer, size - 4);
    auto actual_crc = Scanline::_parse_chunk(buffer + size - 4, 4);
    return expected_crc == actual_crc;
}

void img::PNGImage::read_ihdr(char*& buffer) {
    size_t buff_pos = 0;
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    int width = Scanline::_parse_chunk(_chunk_4b, 4);
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    int height = Scanline::_parse_chunk(_chunk_4b, 4);
    _map.expand(Side::bottom, height);
    _map.expand(Side::right, width);

    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    bit_depth = Scanline::_parse_chunk(_chunk_1b, 1);
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    color_type = Scanline::_parse_chunk(_chunk_1b, 1);
    if (color_type == 2) {
        if (bit_depth != 8 && bit_depth != 16) {
            return;
        }
    } else if (color_type == 6) {
        if (bit_depth != 8 && bit_depth != 16) {
            return;
        }
    } else {
        return;
    }
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    compression_method = Scanline::_parse_chunk(_chunk_1b, 1);
    if (compression_method) {
        return;
    }
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    filter_method = Scanline::_parse_chunk(_chunk_1b, 1);
    if (filter_method) {
        return;
    }
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    interlace_method = Scanline::_parse_chunk(_chunk_1b, 1);
    if (interlace_method) {
        return;
    }
}

void img::PNGImage::read_idat(char*& buffer, size_t size) {}

void img::PNGImage::read(std::string_view path) {

    _status = false;

    Scanline scline {path.data(), ScanMode::read};
    img::PNGImage::Chunk chunk;
    size_t chunk_size;

    // parse 8-bit header

    scline.call_read(8);
    auto smart_buffer = scline.get_chunk(0, 8);
    auto ptr_buffer = smart_buffer.get();
    if (!Scanline::_cmp_chunks(ptr_buffer, 8, _signature, 8)) {
        return;
    }
    return;
    scline.reset_buffer(8);

    // parse IHDR
    scline.call_read(25);
    smart_buffer = scline.get_chunk(0, 8);
    ptr_buffer = smart_buffer.get();
    read_chunk_header(ptr_buffer, chunk, chunk_size);
    if (chunk_size != 13) {
        return;
    }
    if (chunk != img::PNGImage::Chunk::IHDR) {
        return;
    }
    smart_buffer = scline.get_chunk(8, 21);
    ptr_buffer = smart_buffer.get();
    auto start_ptr = ptr_buffer;
    read_ihdr(ptr_buffer);
    if (ptr_buffer - 13 != start_ptr) {
        return;
    }
    smart_buffer = scline.get_chunk(4, 25);
    ptr_buffer = smart_buffer.get();
    if (!read_crc(ptr_buffer, 21)) {
        return;
    }
    scline.reset_buffer(25);

    while (chunk != img::PNGImage::Chunk::IEND) {

        scline.call_read(8);
        smart_buffer = scline.get_chunk(0, 8);
        ptr_buffer = smart_buffer.get();
        read_chunk_header(ptr_buffer, chunk, chunk_size);

        if (chunk == img::PNGImage::Chunk::IDAT) {

        }


        return;
    }
}

void img::PNGImage::write(std::string_view path) {}
