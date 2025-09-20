#pragma once

#include "qfreeimage_global.hpp"

#include <utils/icowriter.hpp>

#include <FreeImage.h>

namespace FreeImageUtils {

inline constexpr auto fibDeleter = [](FIBITMAP *ptr) {
    if (ptr) {
        FreeImage_Unload(ptr);
    }
};
using FIBPtr = std::unique_ptr<FIBITMAP, decltype(fibDeleter)>;

inline FIBPtr makeFIB(FIBITMAP *raw)
{
    return FIBPtr(raw, fibDeleter);
}

QFREEIMAGE_EXPORT auto qImageToFIB(const QImage &qimage) -> FIBPtr;

QFREEIMAGE_EXPORT auto fibToQImage(FIBITMAP *dib) -> QImage;

QFREEIMAGE_EXPORT auto writeIco(const QImage &src,
                                const QString &icoPath,
                                const QColor &squareColor = Qt::transparent,
                                const Utils::ImageSizes &sizes = Utils::defaultIcoSizes) -> bool;

QFREEIMAGE_EXPORT auto readIco(const QString &icoPath) -> Utils::Images;

} // namespace FreeImageUtils
