#include "drawable_common.h"

void DrawableCommon::initShadar(QOpenGLShaderProgram* shader,const QString& shaderName)
{
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/v" + shaderName))
        throw std::runtime_error("could not find glsl vertex shader file");

    // Compile fragment shader
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/f" + shaderName))
        throw std::runtime_error("could not find glsl Fragment shader file");
    // Link shader pipeline
    if (!shader->link())
        throw std::runtime_error("could not link shader");
}
