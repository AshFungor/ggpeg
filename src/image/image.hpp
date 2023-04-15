#include <string>
#include <cstddef>
#include <vector>
#pragma once

namespace img {

    // Code for image class and its supportive structures.

    /** \brief Enumeration that represents sides of the matrix.
     * Used in expanding and trimming functions to specify the direction
     * of these operations.
     */
    enum class Side {
        left,   ///> Left side of the matrix.
        top,    ///> Top side of the matrix.
        right,  ///> Right side of the matrix.
        bottom, ///> Bottom side of the matrix.
    };

    /** \brief Enumeration that represents joint sides of the matrix.
     * Used in more efficient variants of expanding and trimming functions to
     * specify the axis of these operations, rather than just one side.
     */
    enum class JointSide {
        bottom_and_top, ///> vertical (Y) axis.
        left_and_right  ///> horizontal (X) axis.
    };

    /** \brief Class for storing colors with RGB palette.
     * Provides memory-efficient (4-8 bytes per object) color
     * storing supplied with cheap member functions to operate
     * the palette.
     */
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
        /** \brief Get red component of the color.
         * \return value in range [0, 256) which represents
         * red component in the color.
         */
        int R() const;
        /** \brief Get green component of the color.
         * \return value in range [0, 256) which represents
         * green component in the color.
         */
        int G() const;
        /** \brief Get blue component of the color.
         * \return value in range [0, 256) which represents
         * blue component in the color.
         */
        int B() const;
        /** \brief Set red component of the color.
         * \param value number in range [0, 256) to be set.
         * This method does not perform range-checking for \a value, which
         * means that passing this value outside of valid range puts
         * red color component into unspecified state.
         */
        void R(int value);
        /** \brief Set green component of the color.
         * \param value number in range [0, 256) to be set.
         * This method does not perform range-checking for \a value, which
         * means that passing this value outside of valid range puts
         * green color component into unspecified state.
         */
        void G(int value);
        /** \brief Set blue component of the color.
         * \param value number in range [0, 256) to be set.
         * This method does not perform range-checking for \a value, which
         * means that passing this value outside of valid range puts
         * blue color component into unspecified state.
         */
        void B(int value);
        bool operator==(Color other) const;
        /** \brief Default constructor.
         * Initializes white color (0xFFFFFF).
         */
        Color();
        /** \brief Constructor with three color components.
         * \param r red component.
         * \param g green component.
         * \param b blue component.
         * Initializes color from three color components.
         */
        Color(int r, int g, int b);
    };

    class PixelMap {
        using pixel_map_t = std::vector<std::vector<Color>>;
        pixel_map_t _map;
        size_t _width;
        size_t _height;
    public:
        /** \brief Trim the map by number of pixels from one side.
         * \param side side that is trimmed.
         * \param count number of pixels to cut.
         * This function does not perform any range-checking for \a count. Also,
         * if trimming needs to be performed to any of the opposite sides, prefer
         * overload with \a JointSide parameter instead of Side.
         * \see void trim()
         */
        void trim(Side side, int count);
        /** \brief Trim the map by number of pixels from two horizontal or vertical sides.
         * \param sides axis that is trimmed.
         * \param count_1 number of pixels to cut from one side.
         * \param count_2 number of pixels to cat from another side.
         * This function does not perform any range-checking for \a count. Note,
         * \a JointSides values specify how \a count_1 and \a count_2 are treated.
         * \example trim(JointSide::bottom_and_top, 1, 2) means that 1 pixel
         * is cut from the bottom and 2 from the top.
         */
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

