// std headers.
#include <stdexcept>
#include <format>

#include "image.hpp"

using img::PPMImage;
using img::PNGImage;

// PPM Exceptions.
img::PPMImage::DecoderError::DecoderError(ErrorType error_type, std::string actual) {
    switch (error_type) {
    case ErrorType::BadSignature:
        m_error = std::format(fmt_bad_signature, actual);
        break;
    case ErrorType::BadImageSize:
        m_error = std::format(fmt_bad_size, actual);
        break;
    case ErrorType::BadMaxPixelValue:
        m_error = std::format(fmt_bad_max_color, actual);
        break;
    case ErrorType::BadImageData:
        m_error = std::format(fmt_bad_data, actual);
        break;
    }
}

// PNG Exceptions.
// General exceptions.
img::PNGImage::DecoderError::DecoderError(ErrorType error_type, std::string actual) {
    switch (error_type) {
    case ErrorType::BadSignature:
        m_error = std::format(fmt_bad_signature, actual);
        break;
    case ErrorType::BadImageData:
        m_error = std::format(fmt_bad_data, actual);
        break;
    case ErrorType::BadCRC:
        m_error = std::format(fmt_bad_crc, actual);
        break;
    case ErrorType::BadDeflateCompression:
        m_error = std::format(fmt_bad_compression, actual);
        break;
    case ErrorType::BadChunkOrder:
        m_error = std::format(fmt_bad_chunk_order, actual);
        break;
    }
}

// IHDR exceptions.
img::PNGImage::IHDRDecoderError::IHDRDecoderError(IHDRErrorType error_type, std::string actual) {
    switch (error_type) {
    case IHDRErrorType::BadColorType:
        m_error = std::format(fmt_bad_color_type, actual);
        break;
    case IHDRErrorType::BadBitDepth:
        m_error = std::format(fmt_bad_bit_depth, actual);
        break;
    case IHDRErrorType::BadCompession:
        m_error = std::format(fmt_bad_compression, actual);
        break;
    case IHDRErrorType::BadFilter:
        m_error = std::format(fmt_bad_filter, actual);
        break;
    case IHDRErrorType::BadInterlace:
        m_error = std::format(fmt_bad_interlace, actual);
        break;
    case IHDRErrorType::BadImageSize:
        m_error = std::format(fmt_bad_size, actual);
        break;
    }
}

const char * img::PPMImage::DecoderError::what() const noexcept {
    return m_error.data();
}

const char * img::PNGImage::DecoderError::what() const noexcept {
    return m_error.data();
}

const char * img::PNGImage::IHDRDecoderError::what() const noexcept {
    return m_error.data();
}
