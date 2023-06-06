#include "openglview.hpp"

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QtWidgets>

namespace OpenglGraphics {

class OpenglView::OpenglViewPrivate
{
public:
    OpenglViewPrivate(OpenglView *q)
        : q_ptr(q)
    {
        transform.setToIdentity();
        menu = new QMenu(q_ptr);
    }
    ~OpenglViewPrivate() {}

    OpenglView *q_ptr;

    QScopedPointer<QOpenGLShaderProgram> programPtr;
    GLuint texture;
    QOpenGLBuffer vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    QImage image;
    bool isImageChanged = false;
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
    createPopMenu();
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
    d_ptr->vbo.destroy();
    d_ptr->ebo.destroy();
    glDeleteTextures(1, &d_ptr->texture);
    doneCurrent();
}

void OpenglView::setImageUrl(const QString &imageUrl)
{
    QImage image(imageUrl);
    if (image.isNull()) {
        return;
    }

    d_ptr->isImageChanged = true;

    d_ptr->image = image.convertedTo(QImage::Format_RGBA8888_Premultiplied);
    auto size = d_ptr->image.size();
    if (size.width() > width() || size.height() > height()) {
        fitToScreen();
    } else {
        resetToOriginalSize();
    }

    emit imageUrlChanged(imageUrl);
    emit imageSizeChanged(size);
}

void OpenglView::resetToOriginalSize()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    auto size = d_ptr->image.size();
    auto factor_w = qreal(size.width()) / width();
    auto factor_h = qreal(size.height()) / height();
    d_ptr->transform.setToIdentity();
    d_ptr->transform.scale(factor_w, factor_h, 1.0);
    emit emitScaleFactor();

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void OpenglView::fitToScreen()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    auto size = d_ptr->image.size();
    auto factor_w = width() / qreal(size.width());
    auto factor_h = height() / qreal(size.height());
    auto factor = qMin(factor_w, factor_h);
    d_ptr->transform.setToIdentity();
    d_ptr->transform.scale(factor / factor_w, factor / factor_h, 1.0);
    emit emitScaleFactor();

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void OpenglView::rotateNinetieth()
{
    d_ptr->transform.rotate(90, 0, 0, 1);

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void OpenglView::anti_rotateNinetieth()
{
    d_ptr->transform.rotate(-90, 0, 0, 1);

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void OpenglView::initializeGL()
{
    initializeOpenGLFunctions();

    clear();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    d_ptr->programPtr.reset(new QOpenGLShaderProgram(this));
    d_ptr->programPtr->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/texture.vs");
    d_ptr->programPtr->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/texture.fs");
    d_ptr->programPtr->link();
    d_ptr->programPtr->bind();

    initVbo();
    initTexture();

    d_ptr->programPtr->release();
}

void OpenglView::resizeGL(int w, int h)
{
    auto ratioF = devicePixelRatioF();
    glViewport(0, 0, w * ratioF, w * ratioF);

    if (d_ptr->windowSize.isValid()) {
        auto factor_w = d_ptr->windowSize.width() / qreal(w);
        auto factor_h = d_ptr->windowSize.height() / qreal(h);
        d_ptr->transform.scale(factor_w, factor_h, 1.0);
        //qDebug() << "resizeGL" << factor_w << factor_h;
        emit emitScaleFactor();
    }
    d_ptr->windowSize = QSize(w, h);

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
}

void OpenglView::paintGL()
{
    if (d_ptr->image.isNull()) {
        return;
    }

    clear();

    d_ptr->programPtr->bind();

    glActiveTexture(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, d_ptr->texture);
    if (d_ptr->isImageChanged) {
        d_ptr->isImageChanged = false;
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     d_ptr->image.width(),
                     d_ptr->image.height(),
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     d_ptr->image.bits());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    d_ptr->programPtr->setUniformValue("transform", d_ptr->transform);
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
    emit emitScaleFactor();

    QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
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

void OpenglView::initVbo()
{
    GLuint posAttr = GLuint(d_ptr->programPtr->attributeLocation("aPos"));
    GLuint texCord = GLuint(d_ptr->programPtr->attributeLocation("aTexCord"));
    float vertices[] = {
        // positions             // texture coords
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top right
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {0, 1, 3, 1, 2, 3};

    d_ptr->vbo.destroy();
    d_ptr->vbo.create();
    d_ptr->vbo.bind();
    d_ptr->vbo.allocate(vertices, sizeof(vertices));

    d_ptr->ebo.destroy();
    d_ptr->ebo.create();
    d_ptr->ebo.bind();
    d_ptr->ebo.allocate(indices, sizeof(indices));

    d_ptr->programPtr->setAttributeBuffer(posAttr, GL_FLOAT, 0, 3, sizeof(float) * 5);
    d_ptr->programPtr->enableAttributeArray(posAttr);
    d_ptr->programPtr->setAttributeBuffer(texCord, GL_FLOAT, 3 * sizeof(float), 2, sizeof(float) * 5);
    d_ptr->programPtr->enableAttributeArray(texCord);
}

void OpenglView::initTexture()
{
    glGenTextures(1, &d_ptr->texture);
    glBindTexture(GL_TEXTURE_2D, d_ptr->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void OpenglView::clear()
{
    glClearColor(d_ptr->backgroundColor.redF(),
                 d_ptr->backgroundColor.greenF(),
                 d_ptr->backgroundColor.blueF(),
                 d_ptr->backgroundColor.alphaF());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenglView::createPopMenu()
{
    d_ptr->menu->clear();
    d_ptr->menu->addAction(tr("Reset to original size"), this, &OpenglView::resetToOriginalSize);
    d_ptr->menu->addAction(tr("Fit to screen"), this, &OpenglView::fitToScreen);
    d_ptr->menu->addAction(tr("Rotate 90"), this, &OpenglView::rotateNinetieth);
    d_ptr->menu->addAction(tr("Anti rotate 90"), this, &OpenglView::anti_rotateNinetieth);
}

void OpenglView::emitScaleFactor()
{
    auto factor = d_ptr->transform.toTransform().m11() * d_ptr->windowSize.width()
                  / d_ptr->image.width();
    emit scaleFactorChanged(factor);
}

} // namespace OpenglGraphics
