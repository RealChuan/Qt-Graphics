#include "gpudata.hpp"

#include <QPainter>

namespace GpuGraphics {

QImage emptyImage()
{
    static QImage image;
    if (image.isNull()) {
        image = QImage(512, 512, QImage::Format_RGBA8888_Premultiplied);
        image.fill(Qt::white);

        const QRect rect(QPoint(0, 0), image.size());

        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(0.5);
        QFont font;
        font.setPixelSize(48);
        painter.setFont(font);
        painter.drawText(rect, "EMPTY", QTextOption(Qt::AlignCenter));
        painter.end();
    }

    return image;
}

} // namespace GpuGraphics
