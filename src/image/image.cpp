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


img::ImageType img::get_type(std::string_view path) {
    char buff[8];

    std::fstream file;
    file.open(path.data(), std::ios::in | std::ios::binary);
    file.read(buff, 8);

    if (file.fail()) {
        return ImageType::Unknown;
    }

    if (Image::Scanline::_cmp_chunks(buff, 8, PNGImage::_signature, 8)) {
        return ImageType::PNG;
    } else if (Image::Scanline::_cmp_chunks(buff, 2, PPMImage::_binary_magic_number, 2)) {
        return ImageType::PPM;
    }
    return ImageType::Unknown;
}

img::Image img::convert(img::Image& img, img::ImageType new_type) {
    if (new_type == img::ImageType::PNG) {
        PNGImage new_img {};
        new_img.get_map() = img.get_map();
        return new_img;
    } else if (new_type == img::ImageType::PPM) {
        PPMImage new_img {};
        new_img.get_map() = img.get_map();
        return new_img;
    } else {
        throw std::runtime_error("Invalid type; cannot perform conversion");
    }
}
