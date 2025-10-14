
#pragma once

#include "gpugraphics_global.hpp"

#include <QImage>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

namespace GpuGraphics {

class GPUAPHICS OpenglView : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenglView(QWidget *parent = nullptr);
    ~OpenglView() override;

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

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void wheelEvent(QWheelEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    class OpenglViewPrivate;
    QScopedPointer<OpenglViewPrivate> d_ptr;
};

} // namespace GpuGraphics
