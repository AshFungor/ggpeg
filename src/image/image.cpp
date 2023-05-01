// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// compression
#include <zlib.h>

// std headers
#include "image.hpp"
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>

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

std::uint64_t img::PNGImage::_parse_bytes(char* bytes, size_t size) {

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

void img::PNGImage::read_ihdr(std::ifstream& file) {
    file.read(_chunk_4b, 4);
    int width = _parse_bytes(_chunk_4b, 4);
    file.read(_chunk_4b, 4);
    int height = _parse_bytes(_chunk_4b, 4);
    _map.expand(Side::bottom, height);
    _map.expand(Side::right, width);

    file.read(_chunk_4b, 1);
    bit_depth = _parse_bytes(_chunk_4b, 1);
    file.read(_chunk_4b, 1);
    color_type = _parse_bytes(_chunk_4b, 1);
    if (color_type == 2) {
        if (bit_depth != 8 || bit_depth != 16) {
            logger->critical("bit depth for true color must be 8 or 16, received: {}",
                             bit_depth);
            return;
        }
    } else if (color_type == 6) {
        if (bit_depth != 8 || bit_depth != 16) {
            logger->critical("bit depth for true color with alpha must be 8 or 16, received: {}",
                             bit_depth);
            return;
        }
    } else {
        logger->critical("unsupported color type: {}", color_type);
        return;
    }
    file.read(_chunk_1b, 1);
    compression_method = _parse_bytes(_chunk_1b, 1);
    if (compression_method) {
        logger->critical("compression method must be 0, received: {}",
                         compression_method);
    }
    file.read(_chunk_1b, 1);
    filter_method = _parse_bytes(_chunk_1b, 1);
    if (filter_method) {
        logger->critical("filter method must be 0, received: {}",
                         filter_method);
    }
    file.read(_chunk_1b, 1);
    interlace_method = _parse_bytes(_chunk_1b, 1);
    if (interlace_method) {
        logger->critical("only <no interlace> is supported, received: {}",
                         interlace_method);
    }
}

void img::PNGImage::read(std::string_view path) {

    _status = false;
    logger->debug("beginning to parse PNG file \'{}\'", path);

    std::ifstream file {path.data(), std::ios::in | std::ios::binary};

    // parse 8-bit header
    file.read(_chunk_8b, 8);
    if (!_cmp_chunks(_chunk_8b, 8, _signature, 8)) {
        logger->critical("signature of PNG file is not met, received: <{}>",
                         _parse_bytes(_chunk_8b, 8));
        return;
    }
    logger->debug("successfully verified header, reading IHDR");

    // parse IHDR
    file.read(_chunk_4b, 4);
    if (_parse_bytes(_chunk_4b, 4) != 13) {
        logger->critical("first chunk must be size 13, received: <{}>",
                         _parse_bytes(_chunk_4b, 4));
        return;
    }

    file.read(_chunk_4b, 4);
    if (!_cmp_chunks(_chunk_4b, 4, _ihdr_name, 4)) {
        logger->critical("first chunk must be IHDR, received: <{}>",
                         _parse_bytes(_chunk_4b, 4));
        return;
    }
    read_ihdr(file);

    bool not_iend = true;
    while (not_iend) {

    }










}

void img::PNGImage::write(std::string_view path) {}

