#include "image.hpp"

int img::Color::R() const { return _data & red >> red_shift; }
int img::Color::G() const { return _data & green >> green_shift; }
int img::Color::B() const { return _data & blue >> blue_shift; }
img::Color::Color() : _data{0x00ffffff} {}

void img::Color::R(int value) {
    data32_t new_value {static_cast<data32_t>(value)};
    new_value = (new_value << red_shift) & red;
    _data = _data & ~red | new_value;
}

void img::Color::G(int value) {
    data32_t new_value {static_cast<data32_t>(value)};
    new_value = (new_value << green_shift) & green;
    _data = _data & ~green | new_value;
}

void img::Color::B(int value) {
    data32_t new_value {static_cast<data32_t>(value)};
    new_value = (new_value << blue_shift) & blue;
    _data = _data & ~red | new_value;
}

img::Color::Color(int r, int g, int b) {
    _data |= static_cast<data32_t>(r) << red_shift & red;
    _data |= static_cast<data32_t>(g) << green_shift & green;
    _data |= static_cast<data32_t>(b) << blue_shift & blue;
}
