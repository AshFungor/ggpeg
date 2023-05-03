#include "processing.hpp"
#include <image/image.hpp>
#include <iostream>
#include <numeric>
#include <math.h>
#include <vector>
#include <array>

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
    int rows_other = pixel_map_other.rows();
    int columns_other = pixel_map_other.columns();

    for(int i = y; i < y + rows_other; ++i){
        for(int j = x; j < x + columns_other; ++j){
            if(i >= 0 && j >= 0 && i < rows && j < columns){
                pixel_map.at(i, j) = pixel_map_other.at(i - y, j - x);
            }
        }
    }
}

void proc::reflex_x(img::Image &img){
    img::PixelMap &pixel_map = img.get_map();
    size_t rows = pixel_map.rows();
    size_t columns = pixel_map.columns();
    img::PixelMap clear_pixel_map(columns, rows);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            clear_pixel_map.at(rows - i - 1, j) = pixel_map.at(i, j);
        }
    }

    pixel_map = clear_pixel_map;
}

void proc::reflex_y(img::Image &img){
    img::PixelMap &pixel_map = img.get_map();
    size_t rows = pixel_map.rows();
    size_t columns = pixel_map.columns();
    img::PixelMap clear_pixel_map(columns, rows);

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            clear_pixel_map.at(i, columns - j - 1) = pixel_map.at(i, j);
        }
    }

    pixel_map = clear_pixel_map;
}

void proc::resize(img::Image &img, double k){
    img::PixelMap &pixel_map = img.get_map();
    size_t rows = pixel_map.rows();
    size_t columns = pixel_map.columns();

    img::PixelMap clear_pixel_map(round(columns * k), round(rows * k));
    size_t clear_rows = clear_pixel_map.rows();
    size_t clear_columns = clear_pixel_map.columns();

    int least_common_multiples = std::lcm(rows, clear_rows);

    double coefficient_new_image = least_common_multiples / clear_rows;
    double coefficient_old_image = least_common_multiples / rows;

    double current_x = 0.0;
    double current_y = 0.0;

    for(int i = 0; i < clear_rows; ++i){
        current_y = coefficient_new_image * i;
        for(int j = 0; j < clear_columns; ++j){
            current_x = coefficient_new_image * j;
            int start_pixel_x = floor(current_x / coefficient_old_image);
            double end_pixel_x = (current_x + coefficient_new_image) / coefficient_old_image;

            if(end_pixel_x == floor(end_pixel_x)){
                end_pixel_x = end_pixel_x - 1;
            }
            else{
                end_pixel_x = floor(end_pixel_x);
            }

            int start_pixel_y = floor(current_y / coefficient_old_image);
            double end_pixel_y = (current_y + coefficient_new_image) / coefficient_old_image;

            if(end_pixel_y == floor(end_pixel_y)){
                end_pixel_y = end_pixel_y - 1;
            }
            else{
                end_pixel_y = floor(end_pixel_y);
            }

            int counter = 0;
            int R = 0;
            int G = 0;
            int B = 0;

            for(int y = start_pixel_y; y <= end_pixel_y; ++y){
                for(int x = start_pixel_x; x <= end_pixel_x; ++x){
                    ++counter;
                    auto pixel_color = pixel_map.at(y, x);
                    R += pixel_color.R();
                    G += pixel_color.G();
                    B += pixel_color.B();
                }
            }
            clear_pixel_map.at(i, j) = img::Color (round(R/counter), round(G/counter), round(B/counter));
        }
    }
    pixel_map = clear_pixel_map;
}

void proc::rotate(img::Image &img, double degrees){
    degrees = -degrees*2*M_PI/360;

    img::PixelMap &pixel_map = img.get_map();
    size_t rows = pixel_map.rows();
    size_t columns = pixel_map.columns();
    img::PixelMap clear_pixel_map(columns, rows);

    std::vector<std::vector<std::array<double, 4>>> pixels;
    pixels.resize(rows);
    for(auto& row: pixels){
        row.resize(columns);
    }

    double middle_x = columns / 2.0;
    double middle_y = rows / 2.0;

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            double x = j - middle_x + 0.5;
            double y = i - middle_y + 0.5;
            double new_x = x * cos(degrees) - y * sin(degrees);
            double new_y = x * sin(degrees) + y * cos(degrees);
            new_x += round(middle_x - 0.5);
            new_y += round(middle_y - 0.5);

            int y_index = new_y;
            int x_index = new_x;

            if(new_x < columns && new_x >= 0 && new_y < rows && new_y >=0){
                auto pixel_color = pixel_map.at(i, j);
                int R = pixel_color.R();
                int G = pixel_color.G();
                int B = pixel_color.B();

                pixels[y_index][x_index][0] += 1;
                pixels[y_index][x_index][1] = (pixels[y_index][x_index][1]*(pixels[y_index][x_index][0] - 1) + R)/pixels[y_index][x_index][0];
                pixels[y_index][x_index][2] = (pixels[y_index][x_index][2]*(pixels[y_index][x_index][0] - 1) + G)/pixels[y_index][x_index][0];
                pixels[y_index][x_index][3] = (pixels[y_index][x_index][3]*(pixels[y_index][x_index][0] - 1) + B)/pixels[y_index][x_index][0];
            }
        }
    }

    for(int i = 0; i < rows; ++i){
        for(int j = 0; j < columns; ++j){
            int R = pixels[i][j][1];
            int G = pixels[i][j][2];
            int B = pixels[i][j][3];
            if(pixels[i][j][0] != 0){
                clear_pixel_map.at(i, j) = img::Color (round(R), round(G), round(B));
            }
            else{
                double sides = 0.0;
                //find top pixel
                if(i - 1 >= 0){
                    R += pixels[i - 1][j][1];
                    G += pixels[i - 1][j][2];
                    B += pixels[i - 1][j][3];
                    ++sides;
                }
                //find bottom pixel
                if(i + 1 < rows){
                    R += pixels[i + 1][j][1];
                    G += pixels[i + 1][j][2];
                    B += pixels[i + 1][j][3];
                    ++sides;
                }
                //find left pixel
                if(j - 1 >= 0){
                    R += pixels[i][j - 1][1];
                    G += pixels[i][j - 1][2];
                    B += pixels[i][j - 1][3];
                    ++sides;
                }
                //find right pixel
                if(i + 1 < columns){
                    R += pixels[i][j + 1][1];
                    G += pixels[i][j + 1][2];
                    B += pixels[i][j + 1][3];
                    ++sides;
                }

                clear_pixel_map.at(i, j) = img::Color (round(R/sides), round(G/sides), round(B/sides));

            }
        }
    }
    pixel_map = clear_pixel_map;
}