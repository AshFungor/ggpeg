#include "image.hpp"
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>

// Definitions for image class and its supportive structures.
img::PixelMap& img::Image::get_pixel_map() { return _map; }
bool img::Image::is_loaded() const { return _loaded; }

void img::Image::read_ppm(const std::string& path) {
    using byte = char;
    std::ifstream file {path, std::ios::in | std::ios::binary};

    std::uint_fast32_t height, width, max_color;
    std::string magic_number {};
    _loaded = false;

    file >> magic_number >> std::ws;
    if (magic_number != ppm_magic_number) {
        return;
    }
    file >> width >> std::ws;
    file >> height >> std::ws;
    file >> max_color >> std::ws;
    _ppm_max_color = max_color;
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

    _loaded = true;
    file.close();
}

void img::Image::write_out_ppm(const std::string& path) {
    using byte = char;
    std::ofstream file {path, std::ios::out | std::ios::binary};
    file << ppm_magic_number << '\n';
    file << _map.columns() << ' ' << _map.rows() << '\n';
    if (_ppm_max_color == -1) {
        for (int row {0}; row < _map.rows(); ++row) {
            for (int column {0}; column < _map.columns(); ++column) {
                auto& color = _map.at(row, column);
                _ppm_max_color = std::max({color.R(), color.G(), color.B()});
            }
        }
    }
    file << _ppm_max_color << '\n';
    std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(_map.rows() * _map.columns() * 3);
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
    file.write(buffer.get(), _map.rows() * _map.columns() * 3);
    file.close();
}

