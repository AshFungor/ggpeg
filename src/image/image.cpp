// logging
#include <spdlog/spdlog.h>
#include "spdlog/sinks/basic_file_sink.h"

// std headers
#include "image.hpp"
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>

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
    std::uint64_t result;

    while (size > 0) {
        curr_byte = reinterpret_cast<std::uint8_t&>(*(bytes + iter++));
        while (mask > 0) {
            result += (curr_byte & mask) << size;
            mask >>= 1; size--;
        }
        mask = 0x80;
    }

    return result;
}

void img::PNGImage::read(std::string_view path) {
    using byte = char;

    _status = false;
    auto logger = spdlog::basic_logger_mt("basic_logger", "read_log.txt");
    logger->debug("beginning to parse PNG file \'{}\'", path);

    std::ifstream file {path.data(), std::ios::in | std::ios::binary};
    char chunk_4b[4] {};
    char chunk_8b[8] {};

    // parse 8-bit header
    file.read(chunk_8b, 8);
    if (!_cmp_chunks(chunk_8b, 8, _signature, 8)) {
        logger->critical("signature of PNG file is not met, received: <{}>",
                         std::string{chunk_8b});
        return;
    }

    // parse IHDR (image header chunk)
    logger->debug("successfully verified header, reading IHDR");
    file.read(chunk_4b, 4);
    if (_parse_bytes(chunk_4b, 4) != 13) {
        logger->critical("first chunk must be size 13, received: <{}>",
                         _parse_bytes(chunk_4b, 4));
        return;
    }

    file.read(chunk_4b, 4);
    if (!_cmp_chunks(chunk_4b, 4, _ihdr_name, 4)) {
        logger->critical("first chunk must be IHDR, received: <{}>",
                         std::string{chunk_4b});
        return;
    }




}

void img::PNGImage::write(std::string_view path) {}

