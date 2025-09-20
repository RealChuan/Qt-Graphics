#include "freeimageutils.hpp"

#include <QDebug>

namespace FreeImageUtils {

auto qImageToFIB(const QImage &qimage) -> FIBPtr
{
    auto tmp = qimage.convertToFormat(QImage::Format_ARGB32);
    const auto w = tmp.width();
    const auto h = tmp.height();

    auto *dib = FreeImage_AllocateT(FIT_BITMAP,
                                    w,
                                    h,
                                    32,
                                    FI_RGBA_RED_MASK,
                                    FI_RGBA_GREEN_MASK,
                                    FI_RGBA_BLUE_MASK);
    if (!dib) {
        return {};
    }

    for (int y = 0; y < h; ++y) {
        auto *qtLine = reinterpret_cast<const QRgb *>(tmp.constScanLine(y));
        auto *fiLine = FreeImage_GetScanLine(dib, h - 1 - y);

        for (int x = 0; x < w; ++x) {
            auto px = qtLine[x];
            fiLine[FI_RGBA_BLUE] = qBlue(px);
            fiLine[FI_RGBA_GREEN] = qGreen(px);
            fiLine[FI_RGBA_RED] = qRed(px);
            fiLine[FI_RGBA_ALPHA] = qAlpha(px);
            fiLine += 4;
        }
    }
    return makeFIB(dib);
}

auto fibToQImage(FIBITMAP *dib) -> QImage
{
    if (!dib) {
        return {};
    }

    auto dib32 = FreeImage_ConvertTo32Bits(dib);
    if (!dib32) {
        return {};
    }

    const auto w = FreeImage_GetWidth(dib32);
    const auto h = FreeImage_GetHeight(dib32);
    QImage img(w, h, QImage::Format_ARGB32);

    for (int y = 0; y < h; ++y) {
        auto *fiLine = FreeImage_GetScanLine(dib32, h - 1 - y);
        auto *qtLine = reinterpret_cast<QRgb *>(img.scanLine(y));

        for (int x = 0; x < w; ++x) {
            BYTE b = fiLine[FI_RGBA_BLUE];
            BYTE g = fiLine[FI_RGBA_GREEN];
            BYTE r = fiLine[FI_RGBA_RED];
            BYTE a = fiLine[FI_RGBA_ALPHA];

            qtLine[x] = qRgba(r, g, b, a);
            fiLine += 4;
        }
    }

    FreeImage_Unload(dib32);
    return img;
}

constexpr auto cleanResource = [](FIMULTIBITMAP *mbm) {
    if (mbm) {
        FreeImage_CloseMultiBitmap(mbm);
    }
    FreeImage_DeInitialise();
};

auto writeIco(const QImage &src,
              const QString &icoPath,
              const QColor &squareColor,
              const Utils::ImageSizes &sizes) -> bool
{
    auto square = Utils::makeSquare(src, squareColor);

    FreeImage_Initialise(true);

    auto *mbm = FreeImage_OpenMultiBitmap(FIF_ICO, icoPath.toLocal8Bit().constData(), true, false);
    auto cleanup = qScopeGuard([mbm]() { cleanResource(mbm); });
    if (!mbm) {
        qWarning() << "Failed to open multi-bitmap for ICO";
        return false;
    }

    for (auto s : std::as_const(sizes)) {
        auto scaled = square.scaled(s, s, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        auto fib = qImageToFIB(scaled);
        if (fib) {
            FreeImage_AppendPage(mbm, fib.get());
        } else {
            qWarning() << "Failed to convert QImage to FIB for size" << s;
        }
    }

    return true;
}

auto readIco(const QString &icoPath) -> Utils::Images
{
    Utils::Images list;
    FreeImage_Initialise(true);
    auto *mbm = FreeImage_OpenMultiBitmap(FIF_ICO, icoPath.toLocal8Bit().constData(), false, true);
    auto cleanup = qScopeGuard([mbm]() { cleanResource(mbm); });

    if (!mbm) {
        qWarning() << "Failed to open multi-bitmap for reading ICO";
        return list;
    }

    int pages = FreeImage_GetPageCount(mbm);
    for (int i = 0; i < pages; ++i) {
        auto *page = FreeImage_LockPage(mbm, i);
        if (page) {
            auto *page32 = FreeImage_ConvertTo32Bits(page);
            if (page32) {
                list << fibToQImage(page32);
                FreeImage_Unload(page32);
            } else {
                qWarning() << "Failed to convert page to 32 bits:" << i;
            }
            FreeImage_UnlockPage(mbm, page, false);
        } else {
            qWarning() << "Failed to lock page:" << i;
        }
    }
    return list;
}

} // namespace FreeImageUtils
