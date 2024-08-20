#pragma once

#include "gpugraphics_global.hpp"

#include <QRhiWidget>

namespace GpuGraphics {

class GPUAPHICS RhiView : public QRhiWidget
{
    Q_OBJECT
public:
    explicit RhiView(QWidget *parent = nullptr);
    ~RhiView() override;

public slots:
    void setImageUrl(const QString &imageUrl);

    void resetToOriginalSize();
    void fitToScreen();

    void rotateNinetieth();
    void anti_rotateNinetieth();

signals:
    void scaleFactorChanged(qreal factor);
    void imageSizeChanged(const QSize &size);
    void imageUrlChanged(const QString &);
    void rhiChanged(const QString &apiName);

protected:
    void initialize(QRhiCommandBuffer *cb) override;
    void render(QRhiCommandBuffer *cb) override;
    void releaseResources() override;

    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    class RhiViewPrivate;
    QScopedPointer<RhiViewPrivate> d_ptr;
};

} // namespace GpuGraphics
