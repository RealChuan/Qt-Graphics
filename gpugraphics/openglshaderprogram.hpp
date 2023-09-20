#pragma once

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

namespace GpuGraphics {

class OpenGLShaderProgram : public QOpenGLShaderProgram, public QOpenGLFunctions
{
public:
    explicit OpenGLShaderProgram(QObject *parent = nullptr);
    ~OpenGLShaderProgram() override;

    void initVertex(const QString &pos, const QString &texCord);

    void clear();

private:
    class OpenGLShaderProgramPrivate;
    QScopedPointer<OpenGLShaderProgramPrivate> d_ptr;
};

} // namespace GpuGraphics
