#pragma once

#include "utils_global.h"

#include <QImage>
#include <QList>

namespace Utils {

using Images = QList<QImage>;
using ImageSizes = QList<int>;

constexpr auto defaultIcoSizes = {256, 128, 64, 48, 32, 24, 16};

UTILS_EXPORT auto makeSquare(const QImage &src, const QColor &background = Qt::transparent)
    -> QImage;

UTILS_EXPORT auto writeIco(const QImage &src,
                           const QString &icoPath,
                           const QColor &squareColor = Qt::transparent,
                           const ImageSizes &sizes = defaultIcoSizes) -> bool;

} // namespace Utils
