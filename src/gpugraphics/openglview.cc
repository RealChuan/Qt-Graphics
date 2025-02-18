#include "openglview.hpp"
#include "gpudata.hpp"
#include "gpustr.hpp"
#include "openglshaderprogram.hpp"

#include <utils/imagecache.hpp>

#include <QOpenGLBuffer>
#include <QtWidgets>

namespace GpuGraphics {

class OpenglView::OpenglViewPrivate
{
public:
    explicit OpenglViewPrivate(OpenglView *q)
        : q_ptr(q)
    {
        transform.setToIdentity();
        createPopMenu();
    }

    ~OpenglViewPrivate() = default;

    void initTexture()
    {
        q_ptr->glGenTextures(1, &texture);
        q_ptr->glBindTexture(GL_TEXTURE_2D, texture);
        q_ptr->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        q_ptr->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        q_ptr->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        q_ptr->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    void uploadTexture()
    {
        q_ptr->makeCurrent();
        programPtr->bind();
        q_ptr->glBindTexture(GL_TEXTURE_2D, texture);
        q_ptr->glTexImage2D(GL_TEXTURE_2D,
                            0,
                            GL_RGBA,
                            image.width(),
                            image.height(),
                            0,
                            GL_RGBA,
                            GL_UNSIGNED_BYTE,
                            image.bits());
        q_ptr->glGenerateMipmap(GL_TEXTURE_2D);
        programPtr->release();
        q_ptr->doneCurrent();
    }

    void updateTransform()
    {
        q_ptr->makeCurrent();
        programPtr->bind();
        programPtr->setUniformValue("transform", transform);
        programPtr->release();
        q_ptr->doneCurrent();
    }

    void clear()
    {
        q_ptr->glClearColor(backgroundColor.redF(),
                            backgroundColor.greenF(),
                            backgroundColor.blueF(),
                            backgroundColor.alphaF());
        q_ptr->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void createPopMenu()
    {
        menu = new QMenu(q_ptr);
        menu->addAction(Tr::tr("Reset to original size"), q_ptr, &OpenglView::resetToOriginalSize);
        menu->addAction(Tr::tr("Fit to screen"), q_ptr, &OpenglView::fitToScreen);
        menu->addAction(Tr::tr("Rotate 90"), q_ptr, &OpenglView::rotateNinetieth);
        menu->addAction(Tr::tr("Anti rotate 90"), q_ptr, &OpenglView::anti_rotateNinetieth);
    }

    void emitScaleFactor()
    {
        updateTransform();
        auto factor = transform.toTransform().m11() * windowSize.width() / image.width();
        emit q_ptr->scaleFactorChanged(factor);
    }

    OpenglView *q_ptr;

    QScopedPointer<OpenGLShaderProgram> programPtr;
    GLuint texture;

    QImage image;
    QColor backgroundColor = Qt::white;

    QMatrix4x4 transform;
    const qreal scaleFactor = 1.2;
    QSize windowSize;

    QMenu *menu;
};

OpenglView::OpenglView(QWidget *parent)
    : QOpenGLWidget(parent)
    , d_ptr(new OpenglViewPrivate(this))
{
    auto format = this->format();
    qInfo() << "OpenGL Version:" << format.minorVersion() << "~" << format.majorVersion();
}

OpenglView::~OpenglView()
{
    if (isValid()) {
        return;
    }
    makeCurrent();
    d_ptr->programPtr.reset();
    glDeleteTextures(1, &d_ptr->texture);
    doneCurrent();
}

void OpenglView::setImageUrl(const QString &imageUrl)
{
    QImage image;
    if (!Utils::ImageCache::instance()->find(imageUrl, image)) {
        if (!imageUrl.isEmpty()) {
            QMessageBox::warning(this,
                                 tr("WARNING"),
                                 tr("Picture failed to open, Url: %1!").arg(imageUrl));
        }
        image = emptyImage();
    }

    d_ptr->image = image.convertedTo(QImage::Format_RGBA8888_Premultiplied);
    auto size = d_ptr->image.size();
    if (size.width() > width() || size.height() > height()) {
        fitToScreen();
    } else {
        resetToOriginalSize();
    }

    d_ptr->uploadTexture();

    emit imageUrlChanged(imageUrl);
    emit imageSizeChanged(size);
}

void OpenglView::resetToOriginalSize()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    auto size = d_ptr->image.size();
    auto factor_w = static_cast<qreal>(size.width()) / width();
    auto factor_h = static_cast<qreal>(size.height()) / height();
    d_ptr->transform.setToIdentity();
    d_ptr->transform.scale(factor_w, factor_h, 1.0);
    d_ptr->emitScaleFactor();

    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void OpenglView::fitToScreen()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    auto size = d_ptr->image.size();
    auto factor_w = static_cast<qreal>(width()) / size.width();
    auto factor_h = static_cast<qreal>(height()) / size.height();
    auto factor = qMin(factor_w, factor_h);
    d_ptr->transform.setToIdentity();
    d_ptr->transform.scale(factor / factor_w, factor / factor_h, 1.0);
    d_ptr->emitScaleFactor();

    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void OpenglView::rotateNinetieth()
{
    d_ptr->transform.rotate(90, 0, 0, 1);
    d_ptr->updateTransform();

    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void OpenglView::anti_rotateNinetieth()
{
    d_ptr->transform.rotate(-90, 0, 0, 1);
    d_ptr->updateTransform();

    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void OpenglView::initializeGL()
{
    initializeOpenGLFunctions();

    d_ptr->clear();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    d_ptr->programPtr.reset(new OpenGLShaderProgram(this));
    d_ptr->programPtr->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/texture.vert");
    d_ptr->programPtr->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/texture.frag");
    d_ptr->programPtr->link();
    d_ptr->programPtr->bind();

    d_ptr->programPtr->initVertex("inPosition", "inTexCoord");
    d_ptr->initTexture();

    d_ptr->programPtr->release();

    QMetaObject::invokeMethod(this, [this] { setImageUrl({}); }, Qt::QueuedConnection);
}

void OpenglView::resizeGL(int w, int h)
{
    auto ratioF = devicePixelRatioF();
    glViewport(0, 0, w * ratioF, w * ratioF);

    if (d_ptr->windowSize.isValid()) {
        auto factor_w = static_cast<qreal>(d_ptr->windowSize.width()) / w;
        auto factor_h = static_cast<qreal>(d_ptr->windowSize.height()) / h;
        d_ptr->transform.scale(factor_w, factor_h, 1.0);
        //qDebug() << "resizeGL" << factor_w << factor_h;
        d_ptr->emitScaleFactor();
    }
    d_ptr->windowSize = QSize(w, h);

    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void OpenglView::paintGL()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    d_ptr->clear();

    d_ptr->programPtr->bind();

    glActiveTexture(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, d_ptr->texture);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    d_ptr->programPtr->release();
}

void OpenglView::wheelEvent(QWheelEvent *event)
{
    QOpenGLWidget::wheelEvent(event);
    if (d_ptr->image.isNull()) {
        return;
    }

    qreal factor = qPow(d_ptr->scaleFactor, event->angleDelta().y() / 240.0);
    d_ptr->transform.scale(factor, factor, 1.0);
    d_ptr->emitScaleFactor();

    QMetaObject::invokeMethod(this, [this] { update(); }, Qt::QueuedConnection);
}

void OpenglView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QOpenGLWidget::mouseDoubleClickEvent(event);
    fitToScreen();
}

void OpenglView::contextMenuEvent(QContextMenuEvent *event)
{
    d_ptr->menu->exec(event->globalPos());
}

} // namespace GpuGraphics
