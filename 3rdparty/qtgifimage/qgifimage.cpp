#include "qgifimage.h"
#include "qgifimage_p.h"

#include <QFile>

QGifImage::QGifImage()
    : d_ptr(new QGifImagePrivate(this))
{}

/*!
    Constructs a gif image and tries to load the image from the
    file with the given \a fileName
*/
QGifImage::QGifImage(const QString &fileName)
    : d_ptr(new QGifImagePrivate(this))
{
    load(fileName);
}

/*!
    Constructs a gif image with the given \a size
*/
QGifImage::QGifImage(const QSize &size)
    : d_ptr(new QGifImagePrivate(this))
{
    d_ptr->canvasSize = size;
}

/*!
    Destroys the gif image and cleans up.
*/
QGifImage::~QGifImage()
{
    delete d_ptr;
}

/*!
    Return global color table.
 */
QVector<QRgb> QGifImage::globalColorTable() const
{
    Q_D(const QGifImage);
    return d->globalColorTable;
}

/*!
    Return background color of the gif canvas. It only makes sense when
    global color table is not empty.
 */
QColor QGifImage::backgroundColor() const
{
    Q_D(const QGifImage);
    return d->bgColor;
}

/*!
    Set the global color table \a colors and background color \a bgColor.
    \a bgColor must be one the color in \a colors.

    Unlike other image formats that support alpha (e.g. png), GIF does not
    support semi-transparent pixels. So the alpha channel of the color table
    will be ignored.
*/
void QGifImage::setGlobalColorTable(const QVector<QRgb> &colors, const QColor &bgColor)
{
    Q_D(QGifImage);
    d->globalColorTable = colors;
    d->bgColor = bgColor;
}

/*!
    Return the default delay in milliseconds. The default value is 1000 ms.

    The time delay can be different for every frame.
*/
int QGifImage::defaultDelay() const
{
    Q_D(const QGifImage);
    return d->defaultDelayTime;
}

/*!
    Set the default \a delay in milliseconds.
*/
void QGifImage::setDefaultDelay(int delay)
{
    Q_D(QGifImage);
    d->defaultDelayTime = delay;
}

/*!
    Return the default transparent color.

    The transparent color can be different for every frame.
*/
QColor QGifImage::defaultTransparentColor() const
{
    Q_D(const QGifImage);
    return d->defaultTransparentColor;
}

/*!
    Set the default transparent \a color.

    Unlike other image formats that support alpha (e.g. png), GIF does
    not support semi-transparent pixels. The way to achieve transparency
    is to set a color that will be transparent when rendering the GIF.
    So, if you set the transparent color to black, the black pixels in
    the gif file will be transparent.
*/
void QGifImage::setDefaultTransparentColor(const QColor &color)
{
    Q_D(QGifImage);
    d->defaultTransparentColor = color;
}

/*!
    Return the loop count.
*/
int QGifImage::loopCount() const
{
    Q_D(const QGifImage);
    return d->loopCount;
}

/*!
    Set the loop count. The default value of \a loop is 0, which means loop forever.
*/
void QGifImage::setLoopCount(int loop)
{
    Q_D(QGifImage);
    d->loopCount = loop;
}

/*!
    Insert the QImage object \a frame at position \a index with \a delay.

    As gif file only support indexed image, so all the \a frame will be converted
    to the QImage::Format_Indexed8 format. Global color table will be used in the
    convertion if it has been set.

    QImage::offset() will be used when insert the QImage to the gif canvas.
*/
void QGifImage::insertFrame(int index, const QImage &frame, int delay)
{
    Q_D(QGifImage);

    QGifFrameInfoData data;
    data.image = frame;
    data.delayTime = delay;
    data.offset = frame.offset();

    d->frameInfos.insert(index, data);
}

/*!
    \overload

    Insert the QImage object \a frame at position \a index with the given \a offset and
    \a delay.

    As gif file only support indexed image, so all the \a frame will be converted
    to the QImage::Format_Indexed8 format. Global color table will be used in the
    convertion if it has been set.
*/
void QGifImage::insertFrame(int index, const QImage &frame, const QPoint &offset, int delay)
{
    Q_D(QGifImage);
    QGifFrameInfoData data;
    data.image = frame;
    data.delayTime = delay;
    data.offset = offset;

    d->frameInfos.insert(index, data);
}

/*!
    Append the QImage object \a frame with \a delay.

    As gif file only support indexed image, so all the \a frame will be converted
    to the QImage::Format_Indexed8 format. Global color table will be used in the
    convertion if it has been set.

    QImage::offset() will be used when insert the QImage to the gif canvas.
*/
void QGifImage::addFrame(const QImage &frame, int delay)
{
    Q_D(QGifImage);

    QGifFrameInfoData data;
    data.image = frame;
    data.delayTime = delay;
    data.offset = frame.offset();

    d->frameInfos.append(data);
}

/*!
    \overload
    Append the QImage object \a frame with the given \a offset and \a delay.
 */
void QGifImage::addFrame(const QImage &frame, const QPoint &offset, int delay)
{
    Q_D(QGifImage);

    QGifFrameInfoData data;
    data.image = frame;
    data.delayTime = delay;
    data.offset = offset;

    d->frameInfos.append(data);
}

/*!
    Return frame count contained in the gif file.
 */
int QGifImage::frameCount() const
{
    Q_D(const QGifImage);
    return d->frameInfos.count();
}

/*!
    Return the image at \a index.
 */
QImage QGifImage::frame(int index) const
{
    Q_D(const QGifImage);
    if (index < 0 || index >= d->frameInfos.size())
        return QImage();

    return d->frameInfos[index].image;
}

/*!
     Return the offset value of the frame at \a index
 */
QPoint QGifImage::frameOffset(int index) const
{
    Q_D(const QGifImage);
    if (index < 0 || index >= d->frameInfos.size())
        return QPoint();

    return d->frameInfos[index].offset;
}

/*!
     Set the \a offset value for the frame at \a index
 */
void QGifImage::setFrameOffset(int index, const QPoint &offset)
{
    Q_D(QGifImage);
    if (index < 0 || index >= d->frameInfos.size())
        return;
    d->frameInfos[index].offset = offset;
}

/*!
     Return the delay value of the frame at \a index
 */
int QGifImage::frameDelay(int index) const
{
    Q_D(const QGifImage);
    if (index < 0 || index >= d->frameInfos.size())
        return -1;

    return d->frameInfos[index].delayTime;
}

/*!
     Set the \a delay value for the frame at \a index
 */
void QGifImage::setFrameDelay(int index, int delay)
{
    Q_D(QGifImage);
    if (index < 0 || index >= d->frameInfos.size())
        return;
    d->frameInfos[index].delayTime = delay;
}

/*!
     Return the transparent color of the frame at \a index
 */
QColor QGifImage::frameTransparentColor(int index) const
{
    Q_D(const QGifImage);
    if (index < 0 || index >= d->frameInfos.size()) {
        return QColor();
    }
    return d->frameInfos[index].transparentColor;
}

/*!
    Sets the transparent \a color of the frame \a index. Unlike other image formats
    that support alpha (e.g. PNG), GIF does not support semi-transparent pixels.
    The way to achieve transparency is to set a color that will be transparent
    when rendering the GIF. So, if you set the transparent color to black, the
    black pixels in your gif file will be transparent.
*/
void QGifImage::setFrameTransparentColor(int index, const QColor &color)
{
    Q_D(QGifImage);
    if (index < 0 || index >= d->frameInfos.size()) {
        return;
    }
    d->frameInfos[index].transparentColor = color;
}

/*!
    Saves the gif image to the file with the given \a fileName.
    \a flags specifies the conversion method when image data transfered
    to file.
    Returns \c true if the image was successfully saved; otherwise
    returns \c false.
    \sa {Image Formats}{Image Formats}
*/
bool QGifImage::save(const QString &fileName, Qt::ImageConversionFlags flags) const
{
    Q_D(const QGifImage);
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        return d->save(&file, flags);
    }

    return false;
}

/*!
    \overload

    This function writes a QImage to the given \a device.
*/
bool QGifImage::save(QIODevice *device) const
{
    Q_D(const QGifImage);
    if (device->openMode() | QIODevice::WriteOnly) {
        return d->save(device);
    }

    return false;
}

/*!
    Loads an gif image from the file with the given \a fileName. Returns \c true if
    the image was successfully loaded; otherwise invalidates the image
    and returns \c false.
*/
bool QGifImage::load(const QString &fileName)
{
    Q_D(QGifImage);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        return d->load(&file);
    }

    return false;
}

/*!
    \overload

    This function reads a gif image from the given \a device. This can,
    for example, be used to load an image directly into a QByteArray.
*/
bool QGifImage::load(QIODevice *device)
{
    Q_D(QGifImage);
    if (device->openMode() | QIODevice::ReadOnly) {
        return d->load(device);
    }

    return false;
}
