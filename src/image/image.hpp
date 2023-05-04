// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// std headers.
#include <string>
#include <cstddef>
#include <vector>
#include <string_view>
#include <fstream>
#include <memory>

// Header guard.
#pragma once


/** \brief Image processing namespace.
 * This namespace provides access to Color, PixelMap and Image classes
 * to supply the user with simple and fast image-manipulation interface.
 */
namespace img {

    // Code for image class and its supportive structures.

    /** \brief Enumeration that represents sides of the matrix.
     * Used in expanding and trimming functions to specify the direction
     * of these operations.
     */
    enum class Side {
        left,   ///< Left side of the matrix.
        top,    ///< Top side of the matrix.
        right,  ///< Right side of the matrix.
        bottom, ///< Bottom side of the matrix.
    };

    /** \brief Enumeration that represents joint sides of the matrix.
     * Used in more efficient variants of expanding and trimming functions to
     * specify the axis of these operations, rather than just one side.
     */
    enum class JointSide {
        bottom_and_top, ///< vertical (Y) axis.
        left_and_right  ///< horizontal (X) axis.
    };

    /** \brief Class for storing colors with RGB palette.
     * Provides memory-efficient (4-8 bytes per object) color
     * storing supplied with cheap member functions to operate
     * RGB palette.
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
         * \details This method does not perform range-checking for \a value, which
         * means that passing this value outside of valid range puts
         * red color component into unspecified state.
         */
        void R(int value);
        /** \brief Set green component of the color.
         * \param value number in range [0, 256) to be set.
         * \details This method does not perform range-checking for \a value, which
         * means that passing this value outside of valid range puts
         * green color component into unspecified state.
         */
        void G(int value);
        /** \brief Set blue component of the color.
         * \param value number in range [0, 256) to be set.
         * \details This method does not perform range-checking for \a value, which
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
         * \details Initializes color from three color components.
         */
        Color(int r, int g, int b);
    };

    /** \brief Class representing matrix of pixels.
     *  Provides member functions to trim and expand the matrix,
     *  alongside default element and dimension access.
     */
    class PixelMap {
        using pixel_map_t = std::vector<std::vector<Color>>;
        pixel_map_t _map;
        size_t _width;
        size_t _height;
        PixelMap() = default;
    public:
        /** \brief Trim the map by number of pixels from one side.
         * \param side side that is trimmed.
         * \param count number of pixels to cut.
         * \details This function does not perform any range-checking for \a count. Also,
         * if trimming needs to be performed to any pair of the opposite sides, prefer
         * overload with \a JointSide parameter instead of \a Side.
         * \see void trim()
         */
        void trim(Side side, int count);
        /** \brief Trim the map by number of pixels from two joint sides.
         * \param sides axis that is trimmed.
         * \param count_1 number of pixels to cut from one side.
         * \param count_2 number of pixels to cut from another side.
         * \details This function does not perform any range-checking for \a count. Note,
         * \a JointSide values specify how \a count_1 and \a count_2 are treated.
         * For instance, JointSide::bottom_and_top means that \a count_1 pixels
         * are cut from the bottom and \a count_2 from the top.
         */
        void trim(JointSide sides, int count_1, int count_2);
        /** \brief Expand the map by number of pixels on one side.
         * \param side side that is expanded.
         * \param count number of pixels to add.
         * \details Prefer overload with \a JointSide if expanding needs to be done
         * with both opposite sides for efficiency.
         */
        void expand(Side side, int count);
        /** \brief Expand the map by number of pixels on two joint sides.
         * \param sides axis that is expanded.
         * \param count_1 number of pixels to add to one side.
         * \param count_2 number of pixels to add to another side.
         * \details Note, \a JointSide values specify how \a count_1 and \a count_2 are treated.
         * For instance, JointSide::bottom_and_top means that \a count_1 pixels
         * are added to the bottom and \a count_2 to the top.
         */
        void expand(JointSide sides, int count_1, int count_2);
        /** \brief Quick access to pixel at (row, column).
         * \param row row of the pixel.
         * \param column column of the pixel.
         * \return Reference to \a Color object.
         * For performance reasons this function does not perform range checks.
         */
        Color& at(int row, int column);
        /** \brief Get number of rows the map has.
         * \return Number of rows in the map.
         */
        size_t rows() const;
        /** \brief Get number of columns the map has.
         * \return Number of columns in the map.
         */
        size_t columns() const;
        /** \brief Constructs pixel map of designated width and height.
         * \param width number of rows
         * \param height number of columns
         */
        PixelMap(size_t width, size_t height);
    };

    /** \brief Base image class.
     *  Provides basic interface for image class usage.
     */
    class Image {
    protected:
        enum class ScanMode {
            read, write
        };
        class Scanline {
            std::fstream _str {};
            std::unique_ptr<char[]> _buffer {nullptr};
            size_t _buffer_size {0};
            ScanMode _mode;
            // CRC table
            static std::unique_ptr<std::uint32_t[]> _crc_lookup_table;
        public:
            void reset_buffer(size_t number);
            void expand_buffer(size_t number);
            size_t size();
            std::unique_ptr<char[]> get_chunk(size_t start, size_t end);
            void set_chunk(size_t start, size_t end, std::unique_ptr<char[]> chunk);
            void call_read(size_t number);
            void call_write(size_t number);
            char& operator[](size_t index);
            Scanline(std::string_view path, ScanMode mode);
            ~Scanline();
            // helper methods
            static bool _cmp_chunks(const char* chunk_1, size_t size_1,
                                    const char* chunk_2, size_t size_2);
            static std::uint64_t _parse_chunk(char* bytes, size_t size);
            static void _extr_chunk(char** buffer, char* chunk, size_t size);
            static std::uint32_t _crc(char* buffer, size_t size);
        };
        PixelMap _map {0, 0};
        bool _status {false};
        Image() = default;
    public:
        /** \brief Read map of pixels from a file.
         * \param path location of the file
         */
        virtual void read(std::string_view path);
        /** \brief Write map of pixels to new file.
         * \param path location of the new file
         * \details Note, full name of the new file should be
         * provided.
         */
        virtual void write(std::string_view path);
        /** \brief Get direct access to pixel map.
         * \return Reference to underlying pixel map object.
         */
        PixelMap& get_map();
        /** \brief Check if the last read/write operation was successful.
         * \return Status of the last operation.
         */
        bool good();
        virtual ~Image() = default;
    };

    class PPMImage : public Image {
    private:
        constexpr static std::uint_fast64_t _size_limit {5000};
        constexpr static char _binary_magic_number[3] {"P6"};
        constexpr static char _ascii_magic_number[3] {"P3"};
        int _max_color {0};
    public:
        virtual void read(std::string_view path) override;
        virtual void write(std::string_view path) override;
        PPMImage() = default;
    };

    class PNGImage : public Image {
    private:
        /* First byte is 137 (unsigned).
         * Then three bytes are PNG.
         * Then CR + LF
         * Then ^Z + LF
         */
        constexpr static char _signature[9]
        {-119, 80, 78, 71, 13, 10, 26, 10, 0};
        // logger
        std::shared_ptr<spdlog::logger> _logger
            {spdlog::basic_logger_mt("basic_logger", "log.txt")};
        // chunk names
        constexpr static char _iend_name[4] {'I', 'E', 'N', 'D'};
        constexpr static char _idat_name[4] {'I', 'D', 'A', 'T'};
        constexpr static char _ihdr_name[4] {'I', 'H', 'D', 'R'};
        // chunk types
        enum class Chunk {
            IHDR, IDAT, IEND
        };
        // buffers
        static char _chunk_1b[1];
        static char _chunk_4b[4];
        static char _chunk_8b[8];
        // fields of PNG header
        int bit_depth;
        int color_type;
        int compression_method;
        int filter_method;
        int interlace_method;
        // parse functions
        bool read_crc(char* buffer, size_t size);
        void read_chunk_header(char*& buffer, Chunk& chunk, size_t& size);
        void read_ihdr(char*& buffer);
        void read_idat(char*& buffer, size_t size);
    public:
        virtual void read(std::string_view path) override;
        virtual void write(std::string_view path) override;
        PNGImage() = default;
    };
}

