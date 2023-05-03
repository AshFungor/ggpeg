#pragma once
#include <image/image.hpp>

namespace proc {
    // Code for processing functions.
    void negative(img::Image &img);

    void crop(img::Image &img, double left, double top, double right, double bottom);

    void insert(img::Image &img, img::Image &other, int x, int y);

    void reflex_x(img::Image &img);

    void reflex_y(img::Image &img);

    void resize(img::Image &img, double k);

    void rotate(img::Image& img, double degrees);
}
