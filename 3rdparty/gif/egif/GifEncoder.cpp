//
// Created by xiaozhuai on 2020/12/20.
//

#include "GifEncoder.h"
#include "NeuQuant.h"

#include <gif_lib.h>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <stdexcept>
#include <string>
#include <vector>

#define m_gifFile ((GifFileType *) m_gifFileHandler)
#define GifAddExtensionBlockFor(a, func, len, data) \
    GifAddExtensionBlock(&((a)->ExtensionBlockCount), &((a)->ExtensionBlocks), func, len, data)

static void getColorMap(uint8_t *colorMap, const uint8_t *pixels, int nPixels, int quality)
{
    initnet(pixels, nPixels * 3, quality);
    learn();
    unbiasnet();
    inxbuild();
    getcolourmap(colorMap);
}

static void getRasterBits(uint8_t *rasterBits, const uint8_t *pixels, int nPixels)
{
    for (int i = 0; i < nPixels; ++i) {
        rasterBits[i] = inxsearch(pixels[i * 3], pixels[i * 3 + 1], pixels[i * 3 + 2]);
    }
}

inline void RGB2BGR(uint8_t *dst, const uint8_t *src, int width, int height)
{
    for (const uint8_t *dstEnd = dst + width * height * 3; dst < dstEnd; src += 3) {
        *(dst++) = *(src + 2);
        *(dst++) = *(src + 1);
        *(dst++) = *(src);
    }
}

inline void BGRA2BGR(uint8_t *dst, const uint8_t *src, int width, int height)
{
    for (const uint8_t *dstEnd = dst + width * height * 3; dst < dstEnd; src += 4) {
        *(dst++) = *(src);
        *(dst++) = *(src + 1);
        *(dst++) = *(src + 2);
    }
}

inline void RGBA2BGR(uint8_t *dst, const uint8_t *src, int width, int height)
{
    for (const uint8_t *dstEnd = dst + width * height * 3; dst < dstEnd; src += 4) {
        *(dst++) = *(src + 2);
        *(dst++) = *(src + 1);
        *(dst++) = *(src);
    }
}

static auto convertToBGR(
    GifEncoder::PixelFormat format, uint8_t *dst, const uint8_t *src, int width, int height) -> bool
{
    switch (format) {
    case GifEncoder::PIXEL_FORMAT_BGR: memcpy(dst, src, width * height * 3); break;
    case GifEncoder::PIXEL_FORMAT_RGB: RGB2BGR(dst, src, width, height); break;
    case GifEncoder::PIXEL_FORMAT_BGRA: BGRA2BGR(dst, src, width, height); break;
    case GifEncoder::PIXEL_FORMAT_RGBA: RGBA2BGR(dst, src, width, height); break;
    default: return false;
    }
    return true;
}

bool GifEncoder::open(const std::string &file,
                      int width,
                      int height,
                      int quality,
                      bool useGlobalColorMap,
                      int16_t loop,
                      int preAllocSize)
{
    if (m_gifFile != nullptr) {
        return false;
    }

    int error;
    m_gifFileHandler = EGifOpenFileName(file.c_str(), false, &error);
    if (!m_gifFile) {
        return false;
    }

    m_quality = quality;
    m_useGlobalColorMap = useGlobalColorMap;

    reset();

    if (preAllocSize > 0) {
        m_framePixels = (uint8_t *) malloc(preAllocSize);
        m_allocSize = preAllocSize;
    }

    m_gifFile->SWidth = width;
    m_gifFile->SHeight = height;
    m_gifFile->SColorResolution = 8;
    m_gifFile->SBackGroundColor = 0;
    m_gifFile->SColorMap = nullptr;

    uint8_t appExt[11] = {'N', 'E', 'T', 'S', 'C', 'A', 'P', 'E', '2', '.', '0'};
    uint8_t appExtSubBlock[3] = {
        0x01, // hex 0x01
        0x00,
        0x00 // little-endian short. The number of times the loop should be executed.
    };
    memcpy(appExtSubBlock + 1, &loop, sizeof(loop));

    GifAddExtensionBlockFor(m_gifFile, APPLICATION_EXT_FUNC_CODE, sizeof(appExt), appExt);
    GifAddExtensionBlockFor(m_gifFile,
                            CONTINUE_EXT_FUNC_CODE,
                            sizeof(appExtSubBlock),
                            appExtSubBlock);

    return true;
}

bool GifEncoder::push(PixelFormat format, const uint8_t *frame, int width, int height, int delay)
{
    if (m_gifFile == nullptr) {
        return false;
    }

    if (frame == nullptr) {
        return false;
    }

    if (m_useGlobalColorMap) {
        if (isFirstFrame()) {
            m_frameWidth = width;
            m_frameHeight = height;
        } else {
            if (m_frameWidth != width || m_frameHeight != height) {
                throw std::runtime_error("Frame size must be same when use global color map!");
            }
        }

        int needSize = width * height * 3 * (m_frameCount + 1);
        if (m_allocSize < needSize) {
            m_framePixels = (uint8_t *) realloc(m_framePixels, needSize);
            m_allocSize = needSize;
            //            printf("realloc 1\n");
        }
        auto *pixels = m_framePixels + width * height * 3 * m_frameCount;
        convertToBGR(format, pixels, frame, width, height);
        m_allFrameDelays.push_back(delay);
    } else {
        int needSize = width * height * 3;
        if (m_allocSize < needSize) {
            m_framePixels = (uint8_t *) realloc(m_framePixels, needSize);
            m_allocSize = needSize;
            //            printf("realloc 2\n");
        }

        auto *pixels = m_framePixels;
        convertToBGR(format, pixels, frame, width, height);

        auto *colorMap = GifMakeMapObject(256, nullptr);
        getColorMap((uint8_t *) colorMap->Colors, pixels, width * height, m_quality);

        auto *rasterBits = (GifByteType *) malloc(width * height);
        getRasterBits((uint8_t *) rasterBits, pixels, width * height);

        encodeFrame(width, height, delay, colorMap, rasterBits);
    }

    m_frameCount++;

    return true;
}

bool GifEncoder::close()
{
    if (m_gifFile == nullptr) {
        return false;
    }

    ColorMapObject *globalColorMap = nullptr;

    if (m_useGlobalColorMap) {
        globalColorMap = GifMakeMapObject(256, nullptr);
        getColorMap((uint8_t *) globalColorMap->Colors,
                    m_framePixels,
                    m_frameWidth * m_frameHeight * m_frameCount,
                    m_quality);
        m_gifFile->SColorMap = globalColorMap;

        for (int i = 0; i < m_frameCount; ++i) {
            auto *pixels = m_framePixels + m_frameWidth * m_frameHeight * 3 * i;
            auto *rasterBits = (GifByteType *) malloc(m_frameWidth * m_frameHeight);
            getRasterBits((uint8_t *) rasterBits, pixels, m_frameWidth * m_frameHeight);

            encodeFrame(m_frameWidth, m_frameHeight, m_allFrameDelays[i], nullptr, rasterBits);
        }
    }

    int extCount = m_gifFile->ExtensionBlockCount;
    auto *extBlocks = m_gifFile->ExtensionBlocks;

    int savedImageCount = m_gifFile->ImageCount;
    auto *savedImages = m_gifFile->SavedImages;

    int error;
    if (EGifSpew(m_gifFile) == GIF_ERROR) {
        EGifCloseFile(m_gifFile, &error);
        m_gifFileHandler = nullptr;
        return false;
    }

    if (globalColorMap != nullptr) {
        GifFreeMapObject(globalColorMap);
    }

    GifFreeExtensions(&extCount, &extBlocks);
    for (auto *sp = savedImages; sp < savedImages + savedImageCount; sp++) {
        if (sp->ImageDesc.ColorMap != nullptr) {
            GifFreeMapObject(sp->ImageDesc.ColorMap);
            sp->ImageDesc.ColorMap = nullptr;
        }

        if (sp->RasterBits != nullptr) {
            free((char *) sp->RasterBits);
            sp->RasterBits = nullptr;
        }

        GifFreeExtensions(&sp->ExtensionBlockCount, &sp->ExtensionBlocks);
    }
    free(savedImages);

    m_gifFileHandler = nullptr;

    reset();

    return true;
}

void GifEncoder::encodeFrame(int width, int height, int delay, void *colorMap, void *rasterBits)
{
    auto *gifImage = GifMakeSavedImage(m_gifFile, nullptr);

    gifImage->ImageDesc.Left = 0;
    gifImage->ImageDesc.Top = 0;
    gifImage->ImageDesc.Width = width;
    gifImage->ImageDesc.Height = height;
    gifImage->ImageDesc.Interlace = false;
    gifImage->ImageDesc.ColorMap = (ColorMapObject *) colorMap;
    gifImage->RasterBits = (GifByteType *) rasterBits;
    gifImage->ExtensionBlockCount = 0;
    gifImage->ExtensionBlocks = nullptr;

    GraphicsControlBlock gcb;
    gcb.DisposalMode = DISPOSE_DO_NOT;
    gcb.UserInputFlag = false;
    gcb.DelayTime = delay;
    gcb.TransparentColor = NO_TRANSPARENT_COLOR;
    uint8_t gcbBytes[4];
    EGifGCBToExtension(&gcb, gcbBytes);
    GifAddExtensionBlockFor(gifImage, GRAPHICS_EXT_FUNC_CODE, sizeof(gcbBytes), gcbBytes);
}
