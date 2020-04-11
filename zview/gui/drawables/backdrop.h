#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>


class Backdrop : protected QOpenGLFunctions
{
public:

    void setBGcolor();
    Backdrop();    
    void initializeGL();
    void initShader(const QString& shaderName);
    void resizeGL(int w, int h);
    void paintGL();

private:
	QOpenGLShaderProgram m_meshShader;
	QOpenGLBuffer m_verts;
};


