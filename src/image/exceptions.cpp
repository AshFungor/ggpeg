// std headers.
#include <stdexcept>
#include <format>

#include "image.hpp"


// PPM Exceptions.
using img::PPMImage;

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

const char * img::PPMImage::DecoderError::what() const noexcept {
    return m_error.data();
}
