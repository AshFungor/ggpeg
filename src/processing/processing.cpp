#include "processing.hpp"
#include <image/image.hpp>
#include <iostream>

// Definitions for processing functions.

void proc::negative(img::Image &img){
    img::PixelMap &pixel_map = img.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            auto pixel_color = pixel_map.at(i, j);
            int r = pixel_color.R();
            int g = pixel_color.G();
            int b = pixel_color.B();
            pixel_map.at(i, j) = img::Color (255 - r, 255 - g, 255 - b);
        }
    }
}

void proc::crop(img::Image &img, double left, double top, double right, double bottom){
    img::PixelMap &pixel_map = img.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    pixel_map.trim(img::JointSide::left_and_right, round(left/100 * columns), round(right/100 * columns));
    pixel_map.trim(img::JointSide::bottom_and_top, round(bottom/100 * rows), round(top/100 * rows));
}

void proc::insert(img::Image &img, img::Image &other, int x, int y){
    img::PixelMap &pixel_map = img.get_map();
    img::PixelMap &pixel_map_other = other.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();

    for(int i = x; i < rows; ++i){
        for(int j = y; j < columns; ++j){
            pixel_map.at(i, j) = pixel_map_other.at(i - x, j - y);
        }
    }
}