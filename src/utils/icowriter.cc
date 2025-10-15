// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
// Qt-Security score:critical reason:data-parser

/*!
    \class QtIcoHandler
    \since 4.4
    \brief The QtIcoHandler class provides support for the ICO image format.
    \internal
*/

#include "icowriter.hpp"

#include <QBuffer>
#include <QFile>
#include <QPainter>
#include <QtCore>

namespace Utils {

/* 结构体保持 1 字节对齐 */
#pragma pack(push, 1)
struct IconDir
{
    quint16 reserved = 0;
    quint16 type = 0; // 1=icon, 2=cursor
    quint16 count = 0;
};

struct IconDirEntry
{
    quint8 width = 0;
    quint8 height = 0;
    quint8 colorCount = 0;
    quint8 reserved = 0;
    quint16 planes = 0;
    quint16 bitCount = 0;
    quint32 bytesInRes = 0;
    quint32 imageOffset = 0;
};

struct BmpInfoHeader
{
    quint32 size = 40;
    qint32 width = 0;
    qint32 height = 0; // 含 mask，因此是 2*height
    quint16 planes = 1;
    quint16 bitCount = 0;
    quint32 compression = 0;
    quint32 sizeImage = 0;
    qint32 xPelsPerMeter = 0;
    qint32 yPelsPerMeter = 0;
    quint32 clrUsed = 0;
    quint32 clrImportant = 0;
};
#pragma pack(pop)

/* 工具：按行 4-byte 对齐 */
constexpr int bytesPerLine(int bitsPerPixel, int width) noexcept
{
    return ((width * bitsPerPixel + 31) / 32) * 4;
}

template<typename T>
bool writePod(QIODevice *dev, const T &t) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    return dev->write(reinterpret_cast<const char *>(&t), sizeof(t)) == sizeof(t);
}

/* 自下而上写 32-bit BMP 像素 */
bool write32bppPixels(QIODevice *dev, const QImage &img)
{
    const int w = img.width();
    const int h = img.height();
    QByteArray line(w * 4, Qt::Uninitialized);
    for (int y = h - 1; y >= 0; --y) {
        const QRgb *src = reinterpret_cast<const QRgb *>(img.constScanLine(y));
        uchar *dst = reinterpret_cast<uchar *>(line.data());
        for (int x = 0; x < w; ++x) {
            QRgb c = src[x];
            dst[0] = qBlue(c);
            dst[1] = qGreen(c);
            dst[2] = qRed(c);
            dst[3] = qAlpha(c);
            dst += 4;
        }
        if (dev->write(line) != line.size())
            return false;
    }
    return true;
}

/* 生成 1-bit mask，返回写入字节数 */
int writeMask(QIODevice *dev, const QImage &img)
{
    const int w = img.width();
    const int h = img.height();
    const int bpl = bytesPerLine(1, w);
    QByteArray buf(qsizetype(bpl) * qsizetype(h), char(0));
    for (int y = 0; y < h; ++y) {
        const QRgb *sl = reinterpret_cast<const QRgb *>(img.constScanLine(y));
        uchar *m = reinterpret_cast<uchar *>(buf.data() + y * bpl);
        for (int x = 0; x < w; ++x)
            if (qAlpha(sl[x]) > 127)
                m[x >> 3] |= 1 << (7 - (x & 7));
    }
    dev->write(buf);
    return bpl * h;
}

/* ------------------------------------------------------------------ */
/* 写 ICO                                                             */
/* ------------------------------------------------------------------ */
bool writeIco(QIODevice *dev, const Images &images)
{
    if (!dev || !dev->isOpen() || images.empty())
        return false;

    const qint64 base = dev->pos();

    IconDir dir{0, 1, static_cast<quint16>(images.size())};
    if (!writePod(dev, dir))
        return false;

    // 先占位 entry
    QList<IconDirEntry> entries(images.size());
    for (auto &entry : entries) {
        if (!writePod(dev, entry))
            return false;
    }

    // 逐个写图像
    for (size_t i = 0; i < images.size(); ++i) {
        const QImage src = (images[i].width() > 256 || images[i].height() > 256)
                               ? images[i].scaled(256,
                                                  256,
                                                  Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation)
                               : images[i];

        const QImage img32 = src.convertToFormat(QImage::Format_ARGB32);
        const int w = img32.width();
        const int h = img32.height();

        auto &entry = entries[i];
        entry.width = (w == 256 ? 0 : w);
        entry.height = (h == 256 ? 0 : h);
        entry.colorCount = 0;
        entry.reserved = 0;
        entry.planes = 1;
        entry.bitCount = 32;
        entry.imageOffset = dev->pos() - base;

        BmpInfoHeader bmp;
        bmp.width = w;
        bmp.height = h * 2; // 含 mask
        bmp.bitCount = 32;
        bmp.sizeImage = w * h * 4 + bytesPerLine(1, w) * h;

        if (!writePod(dev, bmp) || !write32bppPixels(dev, img32))
            return false;

        writeMask(dev, img32);
        entry.bytesInRes = dev->pos() - base - entry.imageOffset;
    }

    // 回填 entry
    const qint64 endPos = dev->pos();
    if (!dev->seek(base + sizeof(IconDir)))
        return false;

    for (const auto &entry : entries) {
        if (!writePod(dev, entry))
            return false;
    }

    return dev->seek(endPos);
}

QImage makeSquare(const QImage &src, const QColor &background)
{
    if (src.width() == src.height())
        return src;

    const int maxSide = qMax(src.width(), src.height());
    QImage square(maxSide, maxSide, QImage::Format_ARGB32);
    square.fill(background);

    QPainter painter(&square);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage((maxSide - src.width()) / 2, (maxSide - src.height()) / 2, src);
    return square;
}

bool writeIco(const QImage &src,
              const QString &icoPath,
              const QColor &squareColor,
              const ImageSizes &sizes)
{
    auto square = makeSquare(src, squareColor);
    Images images;
    images.reserve(sizes.size());

    for (auto s : sizes) {
        auto scaled = square.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        images.emplaceBack(std::move(scaled));
    }

    QFile file(icoPath);
    if (file.exists() && !file.remove())
        return false;

    if (!file.open(QIODevice::WriteOnly))
        return false;

    return writeIco(&file, images);
}

} // namespace Utils
