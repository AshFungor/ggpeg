#include "image.hpp"
#include <memory>
#include <cstddef>

void img::PNGImage::parse_8b_truecolor(std::uint8_t*& buffer, size_t size) {
    auto step = 3;
    auto window = _map.columns() * step + 1;
    auto current_line = std::make_unique<std::uint8_t[]>(window);
    std::unique_ptr<std::uint8_t[]> upper_line {nullptr};
    for (int row {0}; row < _map.rows(); ++row) {
        for (size_t i {0}; i < window; ++i) {
            current_line[i] = buffer[i];
        }
        buffer += window;
        auto filter = current_line[0];
        switch (filter) {
        case 1:
            reverse_sub(current_line.get() + 1, window - 1);
            break;
        case 2:
            reverse_up(current_line.get() + 1,
                       ((upper_line.get()) ? upper_line.get() + 1 : nullptr),
                       window - 1);
            break;
        case 3:
            reverse_avg(current_line.get() + 1,
                        ((upper_line.get()) ? upper_line.get() + 1 : nullptr),
                        window - 1);
            break;
        case 4:
            reverse_paeth(current_line.get() + 1,
                          ((upper_line.get()) ? upper_line.get() + 1 : nullptr),
                          window - 1);
            break;
        case 0:
            break;
        }
        for (int column {0}; column < _map.columns(); ++column) {
            auto color = img::Color{};
            color.R(current_line[1 + column * step]);
            color.G(current_line[1 + column * step + 1]);
            color.B(current_line[1 + column * step + 2]);
            _map.at(row, column) = color;
        }
        upper_line.reset();
        upper_line = std::move(current_line);
        current_line = std::make_unique<std::uint8_t[]>(window);
    }
}

void img::PNGImage::assemble_8b_truecolor(std::unique_ptr<std::uint8_t[]>& buffer, size_t& size) {
    size = _map.rows() * (_map.columns() * 3 + 1);
    buffer = std::make_unique<std::uint8_t[]>(size);
    std::unique_ptr<std::uint8_t[]> upper {nullptr};
    for (int row {0}; row < _map.rows(); ++row) {
        auto line = std::make_unique<std::uint8_t[]>(_map.columns() * 3 + 1);
        line[0] = 4; // Up filter
        for (int column {0}; column < _map.columns(); ++column) {
            auto color = _map.at(row, column);
            line[1 + column * 3] = color.R();
            line[1 + column * 3 + 1] = color.G();
            line[1 + column * 3 + 2] = color.B();
        }
        auto temp_buff = std::make_unique<std::uint8_t[]>(_map.columns() * 3 + 1);
        for (int i {0}; i < _map.columns() * 3 + 1; ++i) {
            temp_buff[i] = line[i];
        }
        apply_paeth(line.get() + 1, (upper) ? upper.get() + 1 : upper.get(), _map.columns() * 3);
        // apply_sub(line.get() + 1, _map.columns() * 3);
        upper = std::move(temp_buff);
        for (size_t i {row * (_map.columns() * 3 + 1)};
             i < (row + 1) * (_map.columns() * 3 + 1);
             ++i) {
            buffer[i] = line[i - row * (_map.columns() * 3 + 1)];
        }
    }
}
