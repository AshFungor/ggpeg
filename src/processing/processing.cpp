#include "processing.hpp"
#include <image/image.hpp>
#include <iostream>

// Definitions for processing functions.

void proc::negative(img::Image &img){
    img::PixelMap &pixel_map = img.get_map();
    int rows = pixel_map.rows();
    int columns = pixel_map.columns();
    std::cout << rows << " " << columns << std::endl;
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
