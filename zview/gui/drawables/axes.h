#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>



class Axes: public QOpenGLFunctions
{
    
public:

    
    explicit Axes();
       
    void initializeGL();

    void paintGL();

private:
	QOpenGLShaderProgram m_shader;
	QOpenGLBuffer m_verts;
    QOpenGLBuffer m_edges;
};


