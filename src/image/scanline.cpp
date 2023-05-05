#include "image.hpp"

// std headers
#include <fstream>
#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>

std::unique_ptr<std::uint32_t[]> img::Image::Scanline::_crc_lookup_table {nullptr};
char& img::Image::Scanline::operator[](size_t index) { return _buffer[index]; }
size_t img::Image::Scanline::size() { return _buffer_size; }
img::Image::Scanline::~Scanline() { _str.close(); }
img::Image::Scanline::Scanline(std::string_view path, img::Image::ScanMode mode) {
    _mode = mode;
    if (_mode == img::Image::ScanMode::read) {
        _str.open(path.data(), std::ios::binary | std::ios::in);
    } else {
        _str.open(path.data(), std::ios::binary | std::ios::out);
    }
}

void img::Image::Scanline::expand_buffer(size_t number) {
    auto new_buff = std::make_unique<char[]>(_buffer_size + number);
    if (_buffer) {
        for (size_t i {0}; i < _buffer_size; ++i) {
            new_buff[i] = _buffer[i];
        }
    }
    for (size_t i {_buffer_size}; i < number + _buffer_size; ++i) {
        new_buff[i] = 0;
    }
    _buffer.reset();
    _buffer = std::move(new_buff);
    _buffer_size = _buffer_size + number;
}

void img::Image::Scanline::reset_buffer(size_t number) {
    if (_buffer_size == number) {
        _buffer_size = 0;
        _buffer.reset(nullptr);
        return;
    }
    auto new_buff = std::make_unique<char[]>(_buffer_size - number);
    for (size_t i {number}; i < _buffer_size; ++i) {
        new_buff[i - number] = _buffer[i];
    }
    _buffer.reset();
    _buffer = std::move(new_buff);
    _buffer_size = _buffer_size - number;
}

std::unique_ptr<char[]> img::Image::Scanline::get_chunk(size_t start, size_t end) {
    auto new_buff = std::make_unique<char[]>(end - start);
    for (size_t i {start}; i < end; ++i) {
        new_buff[i - start] = _buffer[i];
    }
    return std::move(new_buff);
}

void img::Image::Scanline::set_chunk(size_t start, size_t end, std::unique_ptr<char[]> chunk) {
    for (size_t i {start}; i < end; ++i) {
        _buffer[i] = chunk[i - start];
    }
}

void img::Image::Scanline::call_read(size_t number) {
    assert(_mode == img::Image::ScanMode::read);
    auto buff = std::make_unique<char[]>(number);
    _str.read(buff.get(), number);
    expand_buffer(number);
    for (size_t i {size() - number}; i < size(); ++i) {
        _buffer[i] = buff[i - (size() - number)];
    }
}

void img::Image::Scanline::call_write(size_t number) {
    assert(_mode == img::Image::ScanMode::write);
    auto buff = get_chunk(0, number);
    _str.write(buff.get(), number);
    reset_buffer(number);
}

bool img::Image::Scanline::_cmp_chunks(const char* chunk_1, size_t size_1,
                                       const char* chunk_2, size_t size_2) {
    if (size_1 != size_2) {
        return false;
    }
    bool res = true;
    for (int i {0}; i < size_1; ++i) {
        res &= *(chunk_1 + i) == *(chunk_2 + i);
    }
    return res;
}

std::uint64_t img::Image::Scanline::_parse_chunk(char* bytes, size_t size) {
    std::uint8_t mask {0x80}, curr_byte {};
    size_t iter {0};
    std::uint64_t result {};
    int signed_size = size * 8;

    while (signed_size > 0) {
        curr_byte = reinterpret_cast<std::uint8_t&>(*(bytes + iter++));
        while (mask > 0) {
            result += (curr_byte & mask) << std::max(signed_size - 8, 0);
            mask >>= 1;
        }
        signed_size -= 8;
        mask = 0x80;
    }
    return result;
}

std::uint32_t img::Image::Scanline::_crc(char* buffer, size_t size) {
    if (!_crc_lookup_table) {
        _crc_lookup_table = std::make_unique<std::uint32_t[]>(256);
        std::uint32_t curr;
        int n, k;
        for (n = 0; n < 256; n++) {
            curr = static_cast<std::uint32_t>(n);
            for (k = 0; k < 8; k++) {
                if (curr & 1)
                    curr = 0xedb88320L ^ (curr >> 1);
                else
                    curr = curr >> 1;
            }
            _crc_lookup_table[n] = curr;
        }
    }
    std::uint32_t curr {0xFFFFFFFFL};
    int n;
    for (n = 0; n < size; n++) {
        curr = _crc_lookup_table[(curr ^ buffer[n]) & 0xff] ^ (curr >> 8);
    }
    return curr ^ 0xffffffffL;
}

void img::Image::Scanline::_extr_chunk(char** buffer, char* chunk, size_t size) {
    while (size--) {
        *chunk = **buffer;
        ++chunk;
        ++(*buffer);
    }
}
