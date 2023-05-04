#include "image.hpp"

// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// compression
//#include <zlib.h>

// std headers
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>

// logging macros
#define LOG_CRITICAL_FMT(msg, fmt_args)     \
    _logger->critical(msg, fmt_args);        \
    return

#define LOG_CRITICAL(msg)   \
    _logger->critical(msg);   \
    return


char img::PNGImage::_chunk_1b[1] {};
char img::PNGImage::_chunk_4b[4] {};
char img::PNGImage::_chunk_8b[8] {};


void img::PNGImage::read_chunk_header(char*& buffer,
                                      img::PNGImage::Chunk& chunk,
                                      size_t& size) {
    // first 4-byte chunk is length
    Scanline::_extr_chunk(&buffer, _chunk_4b, 4);
    size = Scanline::_parse_chunk(_chunk_4b, 4);
    // second 4-byte chunk is name
    Scanline::_extr_chunk(&buffer, _chunk_4b, 4);
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
    Scanline::_extr_chunk(&buffer, _chunk_4b, 4);
    int width = Scanline::_parse_chunk(_chunk_4b, 4);
    Scanline::_extr_chunk(&buffer, _chunk_4b, 4);
    int height = Scanline::_parse_chunk(_chunk_4b, 4);
    _map.expand(Side::bottom, height);
    _map.expand(Side::right, width);

    Scanline::_extr_chunk(&buffer, _chunk_1b, 1);
    bit_depth = Scanline::_parse_chunk(_chunk_1b, 1);
    Scanline::_extr_chunk(&buffer, _chunk_1b, 1);
    color_type = Scanline::_parse_chunk(_chunk_1b, 1);
    if (color_type == 2) {
        if (bit_depth != 8 && bit_depth != 16) {
            LOG_CRITICAL_FMT("bit depth for true color must be 8 or 16, received: {}", bit_depth);
        }
    } else if (color_type == 6) {
        if (bit_depth != 8 && bit_depth != 16) {
            LOG_CRITICAL_FMT("bit depth for true color with alpha must be 8 or 16, received: {}",
                                    bit_depth);
        }
    } else {
        LOG_CRITICAL_FMT("unsupported color type: {}", color_type);
    }
    Scanline::_extr_chunk(&buffer, _chunk_1b, 1);
    compression_method = Scanline::_parse_chunk(_chunk_1b, 1);
    if (compression_method) {
        LOG_CRITICAL_FMT("compression method must be 0, received: {}", compression_method);
    }
    Scanline::_extr_chunk(&buffer, _chunk_1b, 1);
    filter_method = Scanline::_parse_chunk(_chunk_1b, 1);
    if (filter_method) {
        LOG_CRITICAL_FMT("filter method must be 0, received: {}", filter_method);
    }
    Scanline::_extr_chunk(&buffer, _chunk_1b, 1);
    interlace_method = Scanline::_parse_chunk(_chunk_1b, 1);
    if (interlace_method) {
        LOG_CRITICAL_FMT("only <no interlace> is supported, received: {}", interlace_method);
    }
}

void img::PNGImage::read_idat(char*& buffer, size_t size) {}

void img::PNGImage::read(std::string_view path) {

    _status = false;
    _logger->debug("beginning to parse PNG file \'{}\'", path);

    std::ifstream file {path.data(), std::ios::in | std::ios::binary};
    img::PNGImage::Chunk chunk;
    size_t chunk_size;

    // buffer for IHDR
    auto buffer_25b = std::make_unique<char[]>(25);
    auto ptr_buffer_25b {buffer_25b.get()};

    // parse 8-bit header
    file.read(_chunk_8b, 8);
    if (!Scanline::_cmp_chunks(_chunk_8b, 8, _signature, 8)) {
        _logger->critical("signature of PNG file is not met, received: <{}>",
                         Scanline::_parse_chunk(_chunk_8b, 8));
        return;
    }
    _logger->debug("successfully verified header, reading IHDR");

    // parse IHDR
    file.read(ptr_buffer_25b, 25);
    read_chunk_header(ptr_buffer_25b, chunk, chunk_size);
    if (chunk_size != 13) {
        LOG_CRITICAL_FMT("IHDR size should be 13, received: {}", chunk_size);
    }
    if (chunk != img::PNGImage::Chunk::IHDR) {
        LOG_CRITICAL_FMT("first chunk type must be IHDR, received: {}", static_cast<int>(chunk));
    }

    read_ihdr(ptr_buffer_25b);
    if (ptr_buffer_25b - 13 - 8 != buffer_25b.get()) {
        LOG_CRITICAL("error occurred, aborting");
    }
    if (!read_crc(ptr_buffer_25b - 17, 21)) {
        LOG_CRITICAL("CRC failed");
    }

    while (chunk != img::PNGImage::Chunk::IEND) {
        ptr_buffer_25b = buffer_25b.get();
        file.read(ptr_buffer_25b, 8);
        read_chunk_header(ptr_buffer_25b, chunk, chunk_size);

        if (chunk == img::PNGImage::Chunk::IDAT) {

        }


        return;
    }
}

void img::PNGImage::write(std::string_view path) {}
