#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
class Shader
{
    QOpenGLShaderProgram m_shader;
public:
    Shader() {}
    bool init(const QString& vshader, const QString& fshader)
    {
                // Compile vertex shader
        if (!m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/"+ vshader+".glsl"))
            return false;

        // Compile fragment shader
        if (!m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/"+ fshader+".glsl"))
            return false;

        // Link shader pipeline
        if (!m_shader.link())
            return false;
        return true;
        
    }
    void setMVP(const QMatrix4x4& mvp)
    {
        m_shader.setUniformValue("mvp_matrix", mvp);
    }
    bool bind()
    {
        if (!m_shader.bind())
            return false;
        return true;

    }
    QOpenGLShaderProgram* get() { return &m_shader; }
};
