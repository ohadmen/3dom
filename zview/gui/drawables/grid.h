#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>



class Grid: public QOpenGLFunctions
{
    static constexpr int n = 51;
public:

    
    explicit Grid();
       
    void initializeGL();

    void paintGL(const QMatrix4x4& mvp);
    void setScale(float s);

private:
	QOpenGLShaderProgram m_shader;
	QOpenGLBuffer m_verts;
    
    float m_scale;
    
    
};


