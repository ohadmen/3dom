#pragma once

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLBuffer>
#include "zview/gui/drawables/drawable_base.h"


class Backdrop : public DrawableBase
{
    
public:

    void setBGcolor();
    explicit Backdrop();
       
    void initializeGL();
    void initShader(const QString& shaderName);
    void resizeGL(int w, int h);
    void paintGL();
    void paintGL(const QMatrix4x4& mvp, int txt);
    QVector3D picking(const QVector3D& p, const QVector3D& n) const;

private:
	QOpenGLShaderProgram m_meshShader;
	QOpenGLBuffer m_verts;
};


