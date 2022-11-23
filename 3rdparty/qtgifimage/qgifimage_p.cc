#include "qgifimage_p.h"

#include <gif_lib.h>
#include <QIODevice>

namespace {

int writeToIODevice(GifFileType *gifFile, const GifByteType *data, int maxSize)
{
    return static_cast<QIODevice *>(gifFile->UserData)
        ->write(reinterpret_cast<const char *>(data), maxSize);
}

int readFromIODevice(GifFileType *gifFile, GifByteType *data, int maxSize)
{
    return static_cast<QIODevice *>(gifFile->UserData)->read(reinterpret_cast<char *>(data), maxSize);
}

} // namespace

QGifFrameInfoData::QGifFrameInfoData()
    : delayTime(-1)
    , interlace(false)
{}

QGifImagePrivate::QGifImagePrivate(QGifImage *p)
    : loopCount(0)
    , defaultDelayTime(1000)
    , q_ptr(p)
{}

QGifImagePrivate::~QGifImagePrivate() {}

QVector<QRgb> QGifImagePrivate::colorTableFromColorMapObject(ColorMapObject *colorMap,
                                                             int transColorIndex) const
{
    QVector<QRgb> colorTable;
    if (colorMap) {
        for (int idx = 0; idx < colorMap->ColorCount; ++idx) {
            GifColorType gifColor = colorMap->Colors[idx];
            QRgb color = gifColor.Blue | (gifColor.Green << 8) | (gifColor.Red << 16);
            // For non-transparent color, set the alpha to opaque.
            if (idx != transColorIndex) {
                color |= 0xff << 24;
            }
            colorTable.append(color);
        }
    }
    return colorTable;
}

ColorMapObject *QGifImagePrivate::colorTableToColorMapObject(QVector<QRgb> colorTable) const
{
    if (colorTable.isEmpty()) {
        return 0;
    }

    ColorMapObject *cmap = (ColorMapObject *) malloc(sizeof(ColorMapObject));
    // num of colors must be a power of 2
    int numColors = 1 << GifBitSize(colorTable.size());
    cmap->ColorCount = numColors;
    //Maybe a bug of giflib, BitsPerPixel is used as size of the color table size.
    cmap->BitsPerPixel = GifBitSize(colorTable.size()); //Todo!
    cmap->SortFlag = false;

    GifColorType *colorValues = (GifColorType *) calloc(numColors, sizeof(GifColorType));
    for (int idx = 0; idx < colorTable.size(); ++idx) {
        colorValues[idx].Red = qRed(colorTable[idx]);
        colorValues[idx].Green = qGreen(colorTable[idx]);
        colorValues[idx].Blue = qBlue(colorTable[idx]);
    }

    cmap->Colors = colorValues;

    return cmap;
}

QSize QGifImagePrivate::getCanvasSize() const
{
    //If canvasSize has been set by user.
    if (canvasSize.isValid()) {
        return canvasSize;
    }

    //Calc the right canvasSize from the frame size.
    int width = -1;
    int height = -1;
    foreach (QGifFrameInfoData info, frameInfos) {
        int w = info.image.width() + info.offset.x();
        int h = info.image.height() + info.offset.y();
        if (w > width) {
            width = w;
        }
        if (h > height) {
            height = h;
        }
    }
    return QSize(width, height);
}

int QGifImagePrivate::getFrameTransparentColorIndex(const QGifFrameInfoData &frameInfo) const
{
    int index = -1;
    QColor transColor = frameInfo.transparentColor.isValid() ? frameInfo.transparentColor
                                                             : defaultTransparentColor;

    if (transColor.isValid()) {
        if (!frameInfo.image.colorTable().isEmpty()) {
            index = frameInfo.image.colorTable().indexOf(transColor.rgb());
        } else if (!globalColorTable.isEmpty()) {
            index = globalColorTable.indexOf(transColor.rgb());
        }
    }
    return index;
}

bool QGifImagePrivate::load(QIODevice *device)
{
    static int interlacedOffset[] = {0, 4, 2, 1}; /* The way Interlaced image should. */
    static int interlacedJumps[] = {8, 8, 4, 2};  /* be read - offsets and jumps... */

    int error;
    GifFileType *gifFile = DGifOpen(device, readFromIODevice, &error);
    if (!gifFile) {
        qWarning(GifErrorString(error));
        return false;
    }

    if (DGifSlurp(gifFile) == GIF_ERROR) {
        return false;
    }

    canvasSize.setWidth(gifFile->SWidth);
    canvasSize.setHeight(gifFile->SHeight);
    if (gifFile->SColorMap) {
        globalColorTable = colorTableFromColorMapObject(gifFile->SColorMap);
        if (gifFile->SBackGroundColor < globalColorTable.size()) {
            bgColor = QColor(globalColorTable[gifFile->SBackGroundColor]);
        }
    }

    for (int idx = 0; idx < gifFile->ImageCount; ++idx) {
        SavedImage gifImage = gifFile->SavedImages[idx];
        int top = gifImage.ImageDesc.Top;
        int left = gifImage.ImageDesc.Left;
        int width = gifImage.ImageDesc.Width;
        int height = gifImage.ImageDesc.Height;

        QGifFrameInfoData frameInfo;
        GraphicsControlBlock gcb;
        DGifSavedExtensionToGCB(gifFile, idx, &gcb);
        int transColorIndex = gcb.TransparentColor;

        QVector<QRgb> colorTable;
        if (gifImage.ImageDesc.ColorMap) {
            colorTable = colorTableFromColorMapObject(gifImage.ImageDesc.ColorMap, transColorIndex);
        } else if (transColorIndex != -1) {
            colorTable = colorTableFromColorMapObject(gifFile->SColorMap, transColorIndex);
        } else {
            colorTable = globalColorTable;
        }

        if (transColorIndex != -1) {
            frameInfo.transparentColor = colorTable[transColorIndex];
        }
        frameInfo.delayTime = gcb.DelayTime * 10; //convert to milliseconds
        frameInfo.interlace = gifImage.ImageDesc.Interlace;
        frameInfo.offset = QPoint(left, top);

        QImage image(width, height, QImage::Format_Indexed8);
        image.setOffset(QPoint(left, top)); //Maybe useful for some users.
        image.setColorTable(colorTable);
        if (transColorIndex != -1) {
            image.fill(transColorIndex);
        } else if (!globalColorTable.isEmpty()) {
            image.fill(gifFile->SBackGroundColor); //!ToDo
        }

        if (gifImage.ImageDesc.Interlace) {
            int line = 0;
            for (int i = 0; i < 4; i++) {
                for (int row = interlacedOffset[i]; row < height; row += interlacedJumps[i]) {
                    memcpy(image.scanLine(row), gifImage.RasterBits + line * width, width);
                    line++;
                }
            }
        } else {
            for (int row = 0; row < height; row++) {
                memcpy(image.scanLine(row), gifImage.RasterBits + row * width, width);
            }
        }

        //Extract other data for the image.
        if (idx == 0) {
            if (gifImage.ExtensionBlockCount > 2) {
                ExtensionBlock *extBlock = gifImage.ExtensionBlocks;
                if (extBlock->Function == APPLICATION_EXT_FUNC_CODE && extBlock->ByteCount == 8) {
                    if (QByteArray((char *) extBlock->Bytes) == QByteArray("NETSCAPE2.0")) {
                        ExtensionBlock *block = gifImage.ExtensionBlocks + 1;
                        if (block->ByteCount == 3) {
                            loopCount = uchar(block->Bytes[1]) + uchar((block->Bytes[2]) << 8);
                        }
                    }
                }
            }
        }

        frameInfo.image = image;
        frameInfos.append(frameInfo);
    }

    DGifCloseFile(gifFile, &error);
    return true;
}

bool QGifImagePrivate::save(QIODevice *device, Qt::ImageConversionFlags flags) const
{
    int error;
    GifFileType *gifFile = EGifOpen(device, writeToIODevice, &error);
    if (!gifFile) {
        qWarning(GifErrorString(error));
        return false;
    }

    QSize _canvasSize = getCanvasSize();
    gifFile->SWidth = _canvasSize.width();
    gifFile->SHeight = _canvasSize.height();
    gifFile->SColorResolution = 8;
    if (!globalColorTable.isEmpty()) {
        gifFile->SColorMap = colorTableToColorMapObject(globalColorTable);
        int idx = globalColorTable.indexOf(bgColor.rgba());
        gifFile->SBackGroundColor = idx == -1 ? 0 : idx;
    }

    gifFile->ImageCount = frameInfos.size();
    gifFile->SavedImages = (SavedImage *) calloc(frameInfos.size(), sizeof(SavedImage));
    for (int idx = 0; idx < frameInfos.size(); ++idx) {
        const QGifFrameInfoData frameInfo = frameInfos.at(idx);
        QImage image = frameInfo.image;
        // 图片格式转换存在噪点
        if (image.format() != QImage::Format_Indexed8) {
            if (!globalColorTable.isEmpty()) {
                image = image.convertToFormat(QImage::Format_Indexed8, globalColorTable, flags);
            } else {
                image = image.convertToFormat(QImage::Format_Indexed8, flags);
            }
        }

        SavedImage *gifImage = gifFile->SavedImages + idx;

        gifImage->ImageDesc.Left = frameInfo.offset.x();
        gifImage->ImageDesc.Top = frameInfo.offset.y();
        gifImage->ImageDesc.Width = image.width();
        gifImage->ImageDesc.Height = image.height();
        gifImage->ImageDesc.Interlace = frameInfo.interlace;

        if (!image.colorTable().isEmpty() && (image.colorTable() != globalColorTable)) {
            gifImage->ImageDesc.ColorMap = colorTableToColorMapObject(image.colorTable());
        } else {
            gifImage->ImageDesc.ColorMap = 0;
        }

        GifByteType *data = (GifByteType *) malloc(image.width() * image.height()
                                                   * sizeof(GifByteType));
        for (int row = 0; row < image.height(); ++row) {
            memcpy(data + row * image.width(), image.scanLine(row), image.width());
        }
        gifImage->RasterBits = data;

        if (idx == 0) {
            uchar data8[12] = "NETSCAPE2.0";
            GifAddExtensionBlock(&gifImage->ExtensionBlockCount,
                                 &gifImage->ExtensionBlocks,
                                 APPLICATION_EXT_FUNC_CODE,
                                 11,
                                 data8);
            uchar data[3];
            data[0] = 0x01;
            data[1] = loopCount & 0xFF;
            data[2] = (loopCount >> 8) & 0xFF;
            GifAddExtensionBlock(&gifImage->ExtensionBlockCount,
                                 &gifImage->ExtensionBlocks,
                                 CONTINUE_EXT_FUNC_CODE,
                                 3,
                                 data);
        }

        GraphicsControlBlock gcbBlock;
        gcbBlock.DisposalMode = 0;
        gcbBlock.UserInputFlag = false;
        gcbBlock.TransparentColor = getFrameTransparentColorIndex(frameInfo);

        if (frameInfo.delayTime != -1) {
            gcbBlock.DelayTime = frameInfo.delayTime / 10; //convert from milliseconds
        } else {
            gcbBlock.DelayTime = defaultDelayTime / 10;
        }

        EGifGCBToSavedExtension(&gcbBlock, gifFile, idx);
    }

    EGifSpew(gifFile);
    //EGifCloseFile(gifFile);

    return true;
}
