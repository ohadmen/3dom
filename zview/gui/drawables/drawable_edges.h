#pragma once
#include "drawable_base.h"

class DrawableEdges : public Types::Edges, public DrawableBase
{
    QOpenGLBuffer m_vBuff; //vertices 3xn
    QOpenGLBuffer m_eBuff; //indices 2xm

public:
    DrawableEdges(const std::string& name);
    ~DrawableEdges();
    void initializeGL();
    void paintGL(const QMatrix4x4& mvp, int txt);
    Types::Roi3d get3dbbox() const;
    QVector3D picking(const QVector3D& p, const QVector3D& n) const;
    
};

