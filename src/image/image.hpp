#include <string>
#include <cstddef>
#include <vector>
#pragma once

namespace img {

    // Code for image class and its supportive structures.

    enum class Side {
        left,
        top,
        right,
        bottom,
    };

    enum class JointSide {
        bottom_and_top,
        left_and_right
    };

    class Color {
        using data32_t = std::uint_least32_t;
        static constexpr data32_t red    {0x00ff0000};
        static constexpr data32_t green  {0x0000ff00};
        static constexpr data32_t blue   {0x000000ff};
        static constexpr data32_t red_shift   {8 * 2};
        static constexpr data32_t green_shift {8 * 1};
        static constexpr data32_t blue_shift      {0};
        data32_t _data;
    public:
        int R() const;
        int G() const;
        int B() const;
        void R(int value);
        void G(int value);
        void B(int value);
        bool operator==(Color other) const;
        Color();
        Color(int r, int g, int b);
    };

    class PixelMap {
        using pixel_map_t = std::vector<std::vector<Color>>;
        pixel_map_t _map;
        size_t _width;
        size_t _height;
    public:
        void trim(Side side, int count);
        void trim(JointSide sides, int count_1, int count_2);
        void expand(Side side, int count);
        void expand(JointSide sides, int count_1, int count_2);
        Color& at(int row, int column);
        size_t rows() const;
        size_t columns() const;
        PixelMap(size_t width, size_t height);
    };

    class Image {
        using status16_t = uint_least16_t;
        PixelMap _map;
        status16_t _status;
        static constexpr status16_t loaded {0x0001};
        // ...

    public:
        int load(const std::string& path);
        int write_out(const std::string& path);
        PixelMap& get_pixel_map();
        bool is_loaded() const;
        // ...
    };
}

