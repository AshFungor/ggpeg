// std headers.
#include <string>
#include <cstdint>
#include <vector>
#include <string_view>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <bitset>
#include <list>
#include <climits>

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
        // Bit flags for color location.
        static constexpr data32_t red    {0x00ff0000};
        static constexpr data32_t green  {0x0000ff00};
        static constexpr data32_t blue   {0x000000ff};
        // Offset of color bits.
        static constexpr data32_t red_shift   {8 * 2};
        static constexpr data32_t green_shift {8 * 1};
        static constexpr data32_t blue_shift      {0};
        // Single 4-byte variable, where RGB data is stored.
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
         * Initializes black color (0x000000).
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
        // Map of pixels.
        pixel_map_t _map;
        // Dimensions of pixel map.
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

    // Type of image
    enum class ImageType {
        PPM,
        PNG,
        Unknown
    };

    // Checks type of image at the following path
    ImageType get_type(std::string_view path);

    /** \brief Base image class.
     *  Provides basic interface for image class usage.
     */
    class Image {
    protected:
        /** \brief Enumeration that represents mode of \a Scanline.
         *  Used to specify mode of Scanline: either read or write.
         */
        enum class ScanMode {
            read,   ///< Read mode.
            write   ///< Write mode.
        };
        /** \brief Class that manages IO operations, provided with internal buffer.
         *  This class offers integrated buffer that can be utilized to make IO operations
         *  more flexible and simple.
         */
        class Scanline {
        private:
            // Underlying file stream.
            std::fstream _str {};
            // Buffered data.
            std::unique_ptr<char[]> _buffer {nullptr};
            // Buffer size.
            size_t _buffer_size {0};
            // Scanline mode (See ScanMode enum).
            ScanMode _mode;
            // CRC table
            static std::unique_ptr<std::uint32_t[]> _crc_lookup_table;
            // Window size
            static constexpr std::uint64_t window_size  {1024 * 32};
            // Single Block size
            static constexpr std::uint64_t block_size   {1024 * 32 - 1};
            // Default compression level
            static constexpr int default_compression    {3};
            // Data node for LZ77 algorithm
            struct triplet {
                std::uint8_t byte {0};
                std::uint32_t offset;
                std::uint32_t length;
            };
            // Needed for range checking
            static char* buffer_start;
            static char* buffer_end;
            // Constant for Adler32 checksum
            static constexpr uint32_t mod_adler {65521};
            // Calculates Adler32 checksum
            static uint32_t adler32(std::uint8_t* data, size_t len);
            // Gets moved buffer after range check
            static std::uint8_t* move_buffer(std::uint8_t* data, std::int32_t number);
            // Finds string match in window and return LZ77 node
            static triplet find_match(std::uint8_t* sliding_window, int size);
            // Performs initial compression in Deflate algorithm
            static std::list<triplet> lz77(std::uint8_t* data, int size,
                                           const std::uint64_t window_size);
            // Matches offset to static Huffman code
            static std::uint32_t match_offset(std::uint32_t offset, int& out_len);
            // Matches length to static Huffman code
            static std::uint32_t match_length(std::uint32_t length, int& out_len);
            // Adds a number to bitset, also moving current pointer by number of bits inserted.
            static void add_bits(std::bitset<block_size * 64>& source, std::uint32_t bits,
                                 int& pos, int number);
        public:
            /** \brief Resets buffer by a number of bytes.
             * \param number defines [0; number) interval to be removed
             */
            void reset_buffer(size_t number);
            /** \brief Expands buffer by a number of bytes.
             * \param number defines how many bytes to be added
             */
            void expand_buffer(size_t number);
            /** \brief Gets size of internal buffer.
             * \return Number of bytes in buffer.
             */
            size_t size();
            /** \brief Gets buffer in [start; end) interval.
             * \param start beginning of new buffer relative to internal
             * \param end last non-included byte relative to internal
             * \return New buffer copied in defined interval.
             */
            std::unique_ptr<char[]> get_chunk(size_t start, size_t end);
            /** \brief Sets buffer in [start; end) interval.
             * \param start beginning of inserted buffer relative to internal
             * \param end last non-included byte relative to internal
             * \param chunk buffer to insert
             */
            void set_chunk(size_t start, size_t end, const char* chunk);
            /** \brief Reads bytes to the end of internal buffer.
             * \param number number of bytes to read
             */
            void call_read(size_t number);
            /** \brief Flushes bytes and resets written bytes in the internal buffer.
             * \param number number of bytes to flush and reset
             */
            void call_write(size_t number);
            /** \brief Returns byte at specified position.
             * \param index position of the byte
             * \return Byte at \a index position.
             */
            char& operator[](size_t index);
            /** \brief Constructor that binds stream to a file for IO operations.
             * \param path path to file
             * \param mode mode of \a Scanline
             */
            Scanline(std::string_view path, ScanMode mode);
            ~Scanline();
            // helper methods (these are not part of actual scanline functionality)
            /** \brief Compares two chunks of data
             * \param chunk_1 first chunk
             * \param size_1 size of first chunk
             * \param chunk_2 second chunk
             * \param size_2 size of second chunk
             * \return Result of the comparison.
             * \details Comparison is done for each respective byte. If at least one pair of them
             * does not match, the result is \a false.
             */
            static bool _cmp_chunks(const char* chunk_1, size_t size_1,
                                    const char* chunk_2, size_t size_2);
            /** \brief Parses byte array into number.
             * \param bytes array to parse
             * \param size size of this array
             * \return A number, which is stored in maximum non-negative integer type.
             */
            static std::uint64_t _parse_chunk(char* bytes, size_t size);
            /** \brief Copies first \a size bytes into another buffer and moves initial for \a size.
             * \param buffer initial buffer
             * \param chunk chunk to fill
             * \param size size to extract from initial
             * \details Gets first \a size bytes from initial buffer, fills chunk with them and
             * moves initial by \a size bytes.
             */
            static void _extr_chunk(char*& buffer, char* chunk, size_t size);
            /** \brief Gets CRC for chunk of size \a size.
             * \param buffer chunk to calculate CRC of
             * \param size size of chunk
             * \return 4-byte CRC.
             */
            static std::uint32_t _crc(const char* buffer, size_t size);
            /** \brief Assembles chunk of bytes with value \a value.
             * \param value value of a new buffer
             * \param size size of new buffer
             * \return Pointer to the buffer, which holds value \a value.
             */
            static std::unique_ptr<char[]> _set_chunk(std::uint64_t value, size_t size);
            // Compression
            static int deflate(char* dest, int& size_out, char* const data, int size);
        };
        // Map of pixels.
        PixelMap _map {0, 0};
        // Status of the last IO operation.
        bool _status {true};
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

        // For access to signature
        friend ImageType get_type(std::string_view path);
    };

    class PPMImage : public Image {
    private:
        // Probably useless.
        constexpr static std::uint_fast64_t _size_limit {5000};
        // PPM file signatures.
        constexpr static char _binary_magic_number[3] {"P6"};
        constexpr static char _ascii_magic_number[3] {"P3"};
        // Max allowed color value.
        int _max_color {255};
    public:
        // These are the same to base class.
        virtual void read(std::string_view path) override;
        virtual void write(std::string_view path) override;
        PPMImage() = default;
        // Error types.
        enum class ErrorType {
            BadSignature,
            BadImageSize,
            BadMaxPixelValue,
            BadImageData
        };

        // Class that represents general problems with decoding PPM.
        class DecoderError : public std::exception {
        private:
            static constexpr char fmt_bad_signature[]
                {"Bad signature, decoded: <{}>. Are you sure this image is PPM?"};
            static constexpr char fmt_bad_size[]
                {"Bad image dimensions, decoded: <{}>"};
            static constexpr char fmt_bad_max_color[]
                {"Bad max pixel value, decoded: <{}>"};
            static constexpr char fmt_bad_data[]
                {"Bad image data, decoder status: <{}>"};
            std::string m_error;
        public:
            DecoderError(ErrorType error_type, std::string actual);
            const char* what() const noexcept override;
        };

        // For access to signature
        friend ImageType get_type(std::string_view path);
    };

    class PNGImage : public Image {
    private:
        /* First byte is 137 (unsigned).
         * Then three bytes are PNG.
         * Then CR + LF
         * Then ^Z + LF
         */
        constexpr static char _signature[8]
        {-119, 80, 78, 71, 13, 10, 26, 10};
        // Chunk names.
        constexpr static char _iend_name[4] {'I', 'E', 'N', 'D'};
        constexpr static char _idat_name[4] {'I', 'D', 'A', 'T'};
        constexpr static char _ihdr_name[4] {'I', 'H', 'D', 'R'};
        // Chunk types.
        enum class Chunk {
            IHDR, IDAT, IEND, Unknown
        };
        // Buffers.
        static char _chunk_1b[1];
        static char _chunk_4b[4];
        static char _chunk_8b[8];
        // Fields of PNG header.
        int bit_depth           {8}; // 1 byte per sample unit (for ex. color component)
        int color_type          {2}; // type of samples
        int compression_method  {0}; // deflate/inflate compression
        int filter_method       {0}; // adaptive filter
        int interlace_method    {0}; // no interlace, default byte order
        int sample_size         {3}; // 3 units in sample
        static constexpr int _size_limit {5000};
        // Parse functions. (chunks)
        // Reads and checks CRC.
        bool read_crc(char* buffer, size_t size);
        // Reads header of a chunk.
        void read_chunk_header(char*& buffer, Chunk& chunk, size_t& size);
        // Reads IHDR data.
        void read_ihdr(char*& buffer);
        // Reads IDAT data.
        void read_idat(char*& buffer, size_t size);
        // Writes IHDR.
        void write_ihdr(Scanline& scanline);
        // Writes IDAT.
        void write_idat(Scanline& scanline);
        // Writes IEND.
        void write_iend(Scanline& scanline);
        // Parse functions. (IDAT)
        // Parses data of 8-bit true color image.
        void parse_8b_truecolor(std::uint8_t*& buffer, size_t size);
        // Assembles data of 8-bit true color image.
        void assemble_8b_truecolor(std::unique_ptr<std::uint8_t[]>& buffer, size_t& size);
        // Filters.
        // Applies Sub filter.
        void apply_sub(std::uint8_t* raw_buffer, size_t size);
        // Applies Up filter.
        void apply_up(std::uint8_t* current_buffer, std::uint8_t* upper_buffer, size_t size);
        // Applies Avg filter.
        void apply_avg(std::uint8_t* current_buffer, std::uint8_t* upper_buffer, size_t size);
        // Applies Paeth filter.
        void apply_paeth(std::uint8_t* current_buffer, std::uint8_t* upper_buffer, size_t size);
        // Reverses Sub filter.
        void reverse_sub(std::uint8_t* processed_buffer, size_t size);
        // Reverses Up filter.
        void reverse_up(std::uint8_t* processed_buffer, std::uint8_t* upper_buffer, size_t size);
        // Reverses Avg filter.
        void reverse_avg(std::uint8_t* processed_buffer, std::uint8_t* upper_buffer, size_t size);
        // Reverses Paeth filter.
        void reverse_paeth(std::uint8_t* processed_buffer, std::uint8_t* upper_buffer, size_t size);
    public:
        // These are the same to base class.
        virtual void read(std::string_view path) override;
        virtual void write(std::string_view path) override;
        PNGImage() = default;

        // Possible general problems with PNG (Including IDAT and IEND chunks).
        enum class ErrorType {
            BadCRC,
            BadSignature,
            BadChunkOrder,
            BadDeflateCompression,
            BadImageData
        };

        // Possible problems with IHDR.
        enum class IHDRErrorType {
            BadImageSize,
            BadBitDepth,
            BadColorType,
            BadCompession,
            BadFilter,
            BadInterlace
        };

        // Class that represents decoding problems with IHDR chunk.
        class IHDRDecoderError : public std::exception {
        private:
            static constexpr char fmt_bad_size[]
                {"Bad image dimensions, decoded: <{}>"};
            static constexpr char fmt_bad_bit_depth[]
                {"Bad bit depth, decoded: <{}>"};
            static constexpr char fmt_bad_color_type[]
                {"Bad color type, decoded: <{}>"};
            static constexpr char fmt_bad_compression[]
                {"Bad compression type, decoded: <{}>"};
            static constexpr char fmt_bad_filter[]
                {"Bad filter type, decoded: <{}>"};
            static constexpr char fmt_bad_interlace[]
                {"Bad interlace, decoded: <{}>"};
            std::string m_error;
        public:
            IHDRDecoderError(IHDRErrorType error_type, std::string actual);
            const char* what() const noexcept override;
        };

        // Class that represents general decoding problems.
        class DecoderError : public std::exception {
        private:
            static constexpr char fmt_bad_crc[]
                {"Bad CRC, decoder message: <{}>"};
            static constexpr char fmt_bad_signature[]
                {"Bad signature, decoded: <{}>. Are you sure this image is PNG?"};
            static constexpr char fmt_bad_chunk_order[]
                {"Bad chunk ordering, decoder message: <{}>"};
            static constexpr char fmt_bad_compression[]
                {"Bad compressed data, decoder message: <{}>"};
            static constexpr char fmt_bad_data[]
                {"Bad image data, decoder message: <{}>"};
            std::string m_error;
        public:
            DecoderError(ErrorType error_type, std::string actual);
            const char* what() const noexcept override;
        };

        // For access to signature
        friend ImageType get_type(std::string_view path);
    };


}

