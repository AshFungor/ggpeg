#include "image.hpp"

// compression
#include <zlib.h>

// std headers
#include <fstream>
#include <string>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>
#include <format>
#include <climits>

char img::PNGImage::_chunk_1b[1] {};
char img::PNGImage::_chunk_4b[4] {};
char img::PNGImage::_chunk_8b[8] {};

#define GET_BUFF(start, end)                        \
    smart_buffer = scline.get_chunk(start, end);    \
    ptr_buffer = smart_buffer.get();

void img::PNGImage::read_chunk_header(char*& buffer,
                                      img::PNGImage::Chunk& chunk,
                                      size_t& size) {
    // first 4-byte chunk is length
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    size = Scanline::_parse_chunk(_chunk_4b, 4);
    // second 4-byte chunk is name
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    if (Scanline::_cmp_chunks(_chunk_4b, 4, _ihdr_name, 4)) {
        chunk = img::PNGImage::Chunk::IHDR;
    } else if (Scanline::_cmp_chunks(_chunk_4b, 4, _idat_name, 4)) {
        chunk = img::PNGImage::Chunk::IDAT;
    } else if (Scanline::_cmp_chunks(_chunk_4b, 4, _iend_name, 4)) {
        chunk = img::PNGImage::Chunk::IEND;
    } else {
        chunk = img::PNGImage::Chunk::Unknown;
    }
}



bool img::PNGImage::read_crc(char* buffer, size_t size) {
    auto expected_crc = Scanline::_crc(buffer, size - 4);
    auto actual_crc = Scanline::_parse_chunk(buffer + size - 4, 4) % UINT32_MAX;
    return expected_crc == actual_crc;
}

void img::PNGImage::read_ihdr(char*& buffer) {
    size_t buff_pos = 0;
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    int width = Scanline::_parse_chunk(_chunk_4b, 4);
    Scanline::_extr_chunk(buffer, _chunk_4b, 4);
    int height = Scanline::_parse_chunk(_chunk_4b, 4);
    if (!(0 <= width && width < _size_limit) || !(0 <= height && height < _size_limit)) {
        throw IHDRDecoderError{IHDRErrorType::BadImageSize,
                               std::format("w: {}, h: {}", width, height)};
    }

    _map.expand(Side::bottom, height);
    _map.expand(Side::right, width);

    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    bit_depth = Scanline::_parse_chunk(_chunk_1b, 1);
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    color_type = Scanline::_parse_chunk(_chunk_1b, 1);
    sample_size = 3;
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    compression_method = Scanline::_parse_chunk(_chunk_1b, 1);
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    filter_method = Scanline::_parse_chunk(_chunk_1b, 1);
    Scanline::_extr_chunk(buffer, _chunk_1b, 1);
    interlace_method = Scanline::_parse_chunk(_chunk_1b, 1);

    if (color_type != 2) {
        throw IHDRDecoderError(IHDRErrorType::BadColorType, std::to_string(color_type));
    }
    if (bit_depth != 8) {
        throw IHDRDecoderError(IHDRErrorType::BadBitDepth, std::to_string(bit_depth));
    }
    if (compression_method) {
        throw IHDRDecoderError(IHDRErrorType::BadCompession, std::to_string(compression_method));
    }
    if (filter_method) {
        throw IHDRDecoderError(IHDRErrorType::BadFilter, std::to_string(filter_method));
    }
    if (interlace_method) {
        throw IHDRDecoderError(IHDRErrorType::BadCompession, std::to_string(interlace_method));
    }
}

void img::PNGImage::read_idat(char*& buffer, size_t size) {
    size_t dest_len {_map.rows() * (_map.columns() + 1) *
                    (3 + (color_type == 6)) *
                    (bit_depth / 8)};
    auto dest = std::make_unique<std::uint8_t[]>(dest_len);
    auto result = uncompress(dest.get(),
                             &dest_len,
                             reinterpret_cast<std::uint8_t*>(buffer),
                             size);
    auto ptr_dest = dest.get();
    if (result != Z_OK) {
        throw DecoderError(ErrorType::BadDeflateCompression, std::format("Status: {}", result));
    }
    parse_8b_truecolor(ptr_dest, dest_len);
}

void img::PNGImage::read(std::string_view path) {

    _status = false;

    Scanline scline {path.data(), ScanMode::read};
    img::PNGImage::Chunk chunk;
    size_t chunk_size;
    _map.trim(Side::bottom, _map.rows());
    _map.trim(Side::right, _map.columns());

    // parse 8-bit header

    scline.call_read(8);
    auto smart_buffer = scline.get_chunk(0, 8);
    auto ptr_buffer = smart_buffer.get();
    if (!Scanline::_cmp_chunks(ptr_buffer, 8, _signature, 8)) {
        throw DecoderError(ErrorType::BadSignature, std::string{ptr_buffer});
    }
    scline.reset_buffer(8);

    // parse IHDR
    scline.call_read(25);
    GET_BUFF(0, 8);
    read_chunk_header(ptr_buffer, chunk, chunk_size);
    if (chunk_size != 13) {
        throw DecoderError(ErrorType::BadChunkOrder,
                           std::format("IHDR size must 13, decoded: {}", chunk_size));
    }
    if (chunk != img::PNGImage::Chunk::IHDR) {
        throw DecoderError(ErrorType::BadChunkOrder,
                           std::format("IHDR expected, decoded (ID) - {}",
                                       static_cast<int>(chunk)));
    }
    GET_BUFF(8, 21);
    read_ihdr(ptr_buffer);
    GET_BUFF(4, 25);
    if (!read_crc(ptr_buffer, 21)) {
        throw DecoderError(ErrorType::BadCRC,
                           std::string{"CRC of IHDR did not match decoded value"});
    }
    scline.reset_buffer(scline.size());

    std::unique_ptr<char[]> data_stream;
    char* ptr_data_stream;
    size_t data_stream_size {0};
    bool is_data_stream {false};

    while (chunk != img::PNGImage::Chunk::IEND) {

        scline.call_read(8);
        GET_BUFF(0, 8);
        read_chunk_header(ptr_buffer, chunk, chunk_size);

        if (is_data_stream && chunk != img::PNGImage::Chunk::IDAT) {
            is_data_stream = false;
            read_idat(ptr_data_stream, data_stream_size);
            data_stream.reset();
        }

        if (chunk == img::PNGImage::Chunk::IDAT) {
            if (!is_data_stream) { is_data_stream = true; }
            scline.call_read(chunk_size + 4);
            GET_BUFF(8, chunk_size + 8);
            auto expanded_stream = std::make_unique<char[]>(data_stream_size + chunk_size);
            size_t i;
            for (i = 0; i < data_stream_size; ++i) {
                expanded_stream[i] = data_stream[i];
            }
            data_stream_size += chunk_size;
            for (size_t initial {i}; i < data_stream_size; ++i) {
                expanded_stream[i] = ptr_buffer[i - initial];
            }
            data_stream = std::move(expanded_stream);
            ptr_data_stream = data_stream.get();
            GET_BUFF(4, chunk_size + 12);
            if (!read_crc(ptr_buffer, chunk_size + 8)) {
                throw DecoderError(ErrorType::BadCRC,
                                   std::string{"CRC of IDAT did not match decoded value"});
            }
            scline.reset_buffer(scline.size());
        } else if (chunk == img::PNGImage::Chunk::IEND) {
            scline.call_read(4);
            GET_BUFF(4, 12);
            if (!read_crc(ptr_buffer, 8)) {
                throw DecoderError(ErrorType::BadCRC,
                                   std::string{"CRC of IEND did not match decoded value"});
            }
            scline.reset_buffer(scline.size());
        } else if (chunk == img::PNGImage::Chunk::Unknown) {
            scline.call_read(chunk_size + 4);
            scline.reset_buffer(scline.size());
        }

    }

    _status = true;
}

void img::PNGImage::write(std::string_view path) {

    _status = false;
    Scanline scline {path.data(), ScanMode::write};

    scline.expand_buffer(8);
    scline.set_chunk(0, 8, _signature);
    scline.call_write(8);

    write_ihdr(scline);
    write_idat(scline);
    write_iend(scline);
    _status = true;
}

void img::PNGImage::write_ihdr(Scanline& scanline) {
    scanline.reset_buffer(scanline.size());
    scanline.expand_buffer(8 + 13 + 4);
    auto buffer = Scanline::_set_chunk(13, 4);
    scanline.set_chunk(0, 4, buffer.get());
    scanline.set_chunk(4, 8, _ihdr_name);
    buffer = Scanline::_set_chunk(_map.columns(), 4);
    scanline.set_chunk(8, 12, buffer.get());
    buffer = Scanline::_set_chunk(_map.rows(), 4);
    scanline.set_chunk(12, 16, buffer.get());
    buffer = Scanline::_set_chunk(8, 1);        // bit depth is always 8
    scanline.set_chunk(16, 17, buffer.get());
    buffer = Scanline::_set_chunk(2, 1);        // color type is RGB
    scanline.set_chunk(17, 18, buffer.get());
    buffer = Scanline::_set_chunk(0, 3);
    scanline.set_chunk(18, 21, buffer.get());
    buffer = scanline.get_chunk(4, 21);
    auto crc = Scanline::_crc(buffer.get(), 21 - 4);
    buffer = Scanline::_set_chunk(crc, 4);
    scanline.set_chunk(21, 25, buffer.get());
    scanline.call_write(scanline.size());
}

void img::PNGImage::write_idat(Scanline& scanline) {
    std::unique_ptr<std::uint8_t[]> data {nullptr};
    size_t size;
    assemble_8b_truecolor(data, size);
    size_t comp_size {static_cast<size_t>(std::ceil(size * 1.2))};
//    int comp_size {static_cast<int>(std::ceil(size * 1.2))};
    auto compressed = std::make_unique<std::uint8_t[]>(comp_size);
    auto result = compress(compressed.get(), &comp_size, data.get(), size);
//    auto result = Scanline::deflate(reinterpret_cast<char*>(compressed.get()),
//                                    comp_size,
//                                    reinterpret_cast<char*>(data.get()),
//                                    size);
    if (result == Z_OK) {
        scanline.reset_buffer(scanline.size());
        scanline.expand_buffer(8 + comp_size + 4);
        auto buffer = Scanline::_set_chunk(comp_size, 4);
        scanline.set_chunk(0, 4, buffer.get());
        scanline.set_chunk(4, 8, _idat_name);
        scanline.set_chunk(8, 8 + comp_size, reinterpret_cast<char*>(compressed.get()));
        buffer = scanline.get_chunk(4, 8 + comp_size);
        auto crc = Scanline::_crc(buffer.get(), comp_size + 4);
        buffer = Scanline::_set_chunk(crc, 4);
        scanline.set_chunk(8 + comp_size, 8 + comp_size + 4, buffer.get());
        scanline.call_write(scanline.size());
    }
}

void img::PNGImage::write_iend(Scanline& scanline) {
    scanline.reset_buffer(scanline.size());
    scanline.expand_buffer(12);
    auto buffer = Scanline::_set_chunk(0, 4);
    scanline.set_chunk(0, 4, buffer.get());
    scanline.set_chunk(4, 8, _iend_name);
    auto crc = Scanline::_crc(_iend_name, 4);
    buffer = Scanline::_set_chunk(crc, 4);
    scanline.set_chunk(8, 12, buffer.get());
    scanline.call_write(scanline.size());
}



