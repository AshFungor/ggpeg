// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// compression
// #include <zlib.h>

// std headers
#include "image.hpp"
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>

// logging macros
#define LOG_CRITICAL(msg, fmt_args)  \
    logger->critical(msg, fmt_args); \
    return

// Definitions for image class and its supportive structures.

img::PixelMap& img::Image::get_map() { return _map; }
bool img::Image::good() { return _status; }
void img::Image::read(std::string_view path) {}
void img::Image::write(std::string_view path) {}

void img::PPMImage::read(std::string_view path) {
    using byte = char;

    std::ifstream file {path.data(), std::ios::in | std::ios::binary};
    std::uint_fast32_t height, width, max_color;
    std::string magic_number {};

    // first the state of object must be discarded
    _status = false;
    _map.trim(img::Side::bottom, _map.rows());
    _max_color = 0;

    file >> magic_number >> std::ws;
    if (magic_number != _binary_magic_number) {
        return;
    }
    file >> width >> std::ws;
    file >> height >> std::ws;
    file >> max_color >> std::ws;
    _max_color = max_color;
    std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(height * width * 3);
    file.read(buffer.get(), height * width * 3);
    _map.expand(Side::bottom, height);
    _map.expand(Side::right, width);
    std::uint8_t red, green, blue;
    for (int row {0}; row < height; ++row) {
        for (int column {0}; column < width; ++column) {
            red = reinterpret_cast<std::uint8_t&>(buffer[row * width * 3 + column * 3]);
            green = reinterpret_cast<std::uint8_t&>(buffer[row * width * 3 + column * 3 + 1]);
            blue = reinterpret_cast<std::uint8_t&>(buffer[row * width * 3 + column * 3 + 2]);
            _map.at(row, column) = Color{red, green, blue};
        }
    }

    _status = true;
    file.close();
}

void img::PPMImage::write(std::string_view path) {
    using byte = char;

    _status = false;

    std::ofstream file {path.data(), std::ios::out | std::ios::binary};
    file << _binary_magic_number << '\n';
    file << _map.columns() << ' ' << _map.rows() << '\n';
    file << _max_color << '\n';

    auto buffer = std::make_unique<byte[]>(_map.rows() * _map.columns() * 3);
    for (int row {0}; row < _map.rows(); ++row) {
        for (int column {0}; column < _map.columns(); ++column) {
            auto& color = _map.at(row, column);
            // needed for reinterpret cast
            std::uint8_t red = color.R();
            std::uint8_t green = color.G();
            std::uint8_t blue = color.B();
            buffer[row * _map.columns() * 3 + column * 3] = reinterpret_cast<byte&>(red);
            buffer[row * _map.columns() * 3 + column * 3 + 1] = reinterpret_cast<byte&>(green);
            buffer[row * _map.columns() * 3 + column * 3 + 2] = reinterpret_cast<byte&>(blue);
        }
    }

    _status = true;
    file.write(buffer.get(), _map.rows() * _map.columns() * 3);
    file.close();
}

bool img::PNGImage::_cmp_chunks(const char* chunk_1, size_t size_1,
                                const char* chunk_2, size_t size_2) {
    if (size_1 != size_2) return false;
    bool res = true;
    for (int i {0}; i < size_1; ++i) {
        res &= *(chunk_1 + i) == *(chunk_2 + i);
    }
    return res;
}

std::uint64_t img::PNGImage::_parse_chunk(char* bytes, size_t size) {

    std::uint8_t mask {0x80}, curr_byte {};
    size_t iter {0};
    std::uint64_t result {};
    int signed_size = size * 8;

    while (signed_size > 0) {
        curr_byte = reinterpret_cast<std::uint8_t&>(*(bytes + iter++));
        while (mask > 0) {
            result += (curr_byte & mask) << std::max(signed_size - 8, 0);
            mask >>= 1;
        }
        signed_size -= 8;
        mask = 0x80;
    }

    return result;
}

void img::PNGImage::read_chunk_header(char* buffer,
                                      img::PNGImage::Chunk& chunk,
                                      size_t& size) {
    // first 4-byte chunk is length
    buffer = _extr_chunk(buffer, _chunk_4b, 4);
    size = _parse_chunk(_chunk_4b, 4);
    // second 4-byte chunk is name
    buffer = _extr_chunk(buffer, _chunk_4b, 4);
    if (_cmp_chunks(_chunk_4b, 4, _ihdr_name, 4)) {
        chunk = img::PNGImage::Chunk::IHDR;
    } else if (_cmp_chunks(_chunk_4b, 4, _idat_name, 4)) {
        chunk = img::PNGImage::Chunk::IDAT;
    } else if (_cmp_chunks(_chunk_4b, 4, _iend_name, 4)) {
        chunk = img::PNGImage::Chunk::IEND;
    }
}

void img::PNGImage::read_ihdr(char* buffer) {
    size_t buff_pos = 0;
    buffer = _extr_chunk(buffer, _chunk_4b, 4);
    int width = _parse_chunk(_chunk_4b, 4);
    buffer = _extr_chunk(buffer, _chunk_4b, 4);
    int height = _parse_chunk(_chunk_4b, 4);
    _map.expand(Side::bottom, height);
    _map.expand(Side::right, width);

    buffer = _extr_chunk(buffer, _chunk_1b, 1);
    bit_depth = _parse_chunk(_chunk_1b, 1);
    buffer = _extr_chunk(buffer, _chunk_1b, 1);
    color_type = _parse_chunk(_chunk_1b, 1);
    if (color_type == 2) {
        if (bit_depth != 8 || bit_depth != 16) {
            LOG_CRITICAL("bit depth for true color must be 8 or 16, received: {}",
                                    bit_depth);
        }
    } else if (color_type == 6) {
        if (bit_depth != 8 || bit_depth != 16) {
            LOG_CRITICAL("bit depth for true color with alpha must be "
                                    "8 or 16, received: {}",
                                    bit_depth);
        }
    } else {
        LOG_CRITICAL("unsupported color type: {}",
                                color_type);
    }
    buffer = _extr_chunk(buffer, _chunk_1b, 1);
    compression_method = _parse_chunk(_chunk_1b, 1);
    if (compression_method) {
        LOG_CRITICAL("compression method must be 0, received: {}",
                                compression_method);
    }
    buffer = _extr_chunk(buffer, _chunk_1b, 1);
    filter_method = _parse_chunk(_chunk_1b, 1);
    if (filter_method) {
        LOG_CRITICAL("filter method must be 0, received: {}",
                                filter_method);
    }
    buffer = _extr_chunk(buffer, _chunk_1b, 1);
    interlace_method = _parse_chunk(_chunk_1b, 1);
    if (interlace_method) {
        LOG_CRITICAL("only <no interlace> is supported, received: {}",
                                interlace_method);
    }
}

void img::PNGImage::read(std::string_view path) {

    _status = false;
    logger->debug("beginning to parse PNG file \'{}\'", path);

    std::ifstream file {path.data(), std::ios::in | std::ios::binary};
    img::PNGImage::Chunk chunk;
    size_t chunk_size;

    // parse 8-bit header
    file.read(_chunk_8b, 8);
    if (!_cmp_chunks(_chunk_8b, 8, _signature, 8)) {
        logger->critical("signature of PNG file is not met, received: <{}>",
                         _parse_chunk(_chunk_8b, 8));
        return;
    }
    logger->debug("successfully verified header, reading IHDR");

    // parse IHDR
    file.read(_chunk_8b, 8);
    read_chunk_header(_chunk_8b, chunk, chunk_size);
    if (chunk_size != 13) {
        LOG_CRITICAL("IHDR size should be 13, received: {}", chunk_size);
    }
    if (chunk != img::PNGImage::Chunk::IHDR) {
        LOG_CRITICAL("first chunk type must be IHDR, received: {}", static_cast<int>(chunk));
    }



    while (chunk != img::PNGImage::Chunk::IEND) {
        file.read(_chunk_8b, 8);
        read_chunk_header(_chunk_8b, chunk, chunk_size);
    }










}

void img::PNGImage::write(std::string_view path) {}

char* img::PNGImage::_extr_chunk(char* buffer, char* chunk, size_t size) {return nullptr;}