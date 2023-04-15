#include "image.hpp"
#include <vector>
#include <stdexcept>

img::PixelMap::PixelMap(size_t width, size_t height) : _width{width}, _height{height} {
    _map = std::move(pixel_map_t(height));
    for (int row {0}; row < height; ++row) {
        _map[row] = std::move(std::vector<Color>(width));
    }
}

size_t img::PixelMap::rows() const   { return _height; }
size_t img::PixelMap::columns() const { return _width; }

img::Color& img::PixelMap::at(int row, int column) {
    return _map[row][column];
}

void img::PixelMap::expand(JointSide sides, int count_1, int count_2) {
    if (sides == JointSide::bottom_and_top) {
        int& top = count_2;
        int& bottom = count_1;
        _height += top + bottom;
        // There is no forward_add_range() function in STL, so
        // we have to improvise.
        _map.resize(_height);
        // std::move() needs third iterator to be out of range that
        // first two provide, so move_backward() needs to be here instead.
        std::move_backward(_map.begin(), _map.end() - top - bottom, _map.end() - bottom);
        std::fill(_map.begin(), _map.begin() + top, std::vector<Color>(_width));
        std::fill(_map.end() - bottom, _map.end(), std::vector<Color>(_width));
    }
    else if (sides == JointSide::left_and_right) {
        int& left = count_2;
        int& right = count_1;
        _width += left + right;
        for (std::vector<Color>& row : _map) {
            row.resize(_width);
            std::move_backward(row.begin(), row.end() - right, row.end());
            std::fill(row.begin(), row.begin() + left, Color{});
            std::fill(row.end() - right, row.end(), Color{});
        }
    }
    else {
        throw std::runtime_error("Parameter <sides> value is out of acceptable range.");
    }
}

void img::PixelMap::expand(Side side, int count) {
    switch(side) {
    case Side::right:
        _width += count;
        for (std::vector<Color>& row : _map) {
            row.resize(_width);
            std::fill(row.end() - count, row.end(), Color{});
        }
        break;
    case Side::bottom:
        _height += count;
        _map.resize(_height);
        std::fill(_map.end() - count, _map.end(), std::vector<Color>(_width));
        break;
    case Side::left:
        _width += count;
        for (std::vector<Color>& row : _map) {
            row.resize(_width);
            std::move_backward(row.begin(), row.end() - count, row.end());
            std::fill(row.begin(), row.begin() + count, Color{});
        }
        break;
    case Side::top:
        _height += count;
        _map.resize(_height);
        std::move_backward(_map.begin(), _map.end() - count, _map.end());
        std::fill(_map.begin(), _map.begin() + count, std::vector<Color>(_width));
        break;
    default:
        throw std::runtime_error("Parameter <side> value is out of acceptable range.");
    }
}


