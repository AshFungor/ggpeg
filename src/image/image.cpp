// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// compression
#include <zlib.h>

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

