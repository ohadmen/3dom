#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>



class Backdrop: public QOpenGLFunctions
{
    
public:

    void setBGcolor();
    explicit Backdrop();
       
    void initializeGL();
    void initShader(const QString& shaderName);

    void paintGL();

private:
	QOpenGLShaderProgram m_meshShader;
	QOpenGLBuffer m_verts;
};


