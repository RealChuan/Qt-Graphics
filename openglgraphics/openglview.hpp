
#pragma once

#include "openglgraphics_global.hpp"

#include <QImage>
#include <QOpenGLFunctions>
#include <QOpenGLWidget>

namespace OpenglGraphics {

class OPENGLGRAPHICS OpenglView : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    OpenglView(QWidget *parent = nullptr);
    ~OpenglView();

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

    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    void initVbo();
    void initTexture();
    void clear();
    void createPopMenu();
    void emitScaleFactor();

    class OpenglViewPrivate;
    QScopedPointer<OpenglViewPrivate> d_ptr;
};

} // namespace OpenglGraphics
