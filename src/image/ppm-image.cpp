// std headers
#include "image.hpp"
#include <fstream>
#include <string>
#include <cstddef>
#include <memory>
#include <algorithm>
#include <format>

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
        throw DecoderError{ErrorType::BadSignature, magic_number};
    }

    // metadata
    file >> width >> std::ws;
    file >> height >> std::ws;
    file >> max_color >> std::ws;
    if (!(0 <= width && width <= _size_limit) || !(0 <= height && height <= _size_limit)) {
        throw DecoderError{ErrorType::BadImageSize, std::format("w: {}, h: {}", width, height)};
    }
    if (max_color > 255) {
        throw DecoderError{ErrorType::BadMaxPixelValue, std::to_string(max_color)};
    }

    // actual image data
    _max_color = max_color;
    std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(height * width * 3);
    file.read(buffer.get(), height * width * 3);
    if (file.gcount() != height * width * 3) {
        throw DecoderError{ErrorType::BadImageData,
                           std::format("only {} bytes extracted out of {}",
                                       file.gcount(),
                                       height * width * 3)};
    }

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
