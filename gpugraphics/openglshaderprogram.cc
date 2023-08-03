#include "openglshaderprogram.hpp"

#include <QOpenGLBuffer>

namespace GpuGraphics {

class OpenGLShaderProgram::OpenGLShaderProgramPrivate
{
public:
    explicit OpenGLShaderProgramPrivate(OpenGLShaderProgram *q)
        : q_ptr(q)
    {}

    OpenGLShaderProgram *q_ptr;

    QOpenGLBuffer vbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    QOpenGLBuffer ebo = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
};

OpenGLShaderProgram::OpenGLShaderProgram(QObject *parent)
    : QOpenGLShaderProgram(parent)
    , d_ptr(new OpenGLShaderProgramPrivate(this))
{}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
    clear();
}

void OpenGLShaderProgram::initVertex(const QString &pos, const QString &texCord)
{
    auto posAttr = attributeLocation(pos);
    auto texCordAttr = attributeLocation(texCord);

    float vertices[] = {
        // positions             // texture coords
        1.0F,  1.0F,  0.0F, 1.0F, 1.0F, // top right
        1.0F,  -1.0F, 0.0F, 1.0F, 0.0F, // bottom right
        -1.0F, -1.0F, 0.0F, 0.0F, 0.0F, // bottom left
        -1.0F, 1.0F,  0.0F, 0.0F, 1.0F  // top left
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

    setAttributeBuffer(posAttr, GL_FLOAT, 0, 3, sizeof(float) * 5);
    enableAttributeArray(posAttr);
    setAttributeBuffer(texCordAttr, GL_FLOAT, 3 * sizeof(float), 2, sizeof(float) * 5);
    enableAttributeArray(texCordAttr);
}

void OpenGLShaderProgram::clear()
{
    d_ptr->vbo.destroy();
    d_ptr->ebo.destroy();
}

} // namespace GpuGraphics
