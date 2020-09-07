#pragma once
#include "drawable_base.h"

class DrawableEdges : public Types::Edges, public DrawableBase
{
    
    QOpenGLBuffer m_eBuff; //indices 2xm

public:
Types::Shape getShape() const override;
    DrawableEdges(const std::string& name);
    ~DrawableEdges();
    void initializeGL();
    void paintGL(const QMatrix4x4& mvp);
    Types::Roi3d get3dbbox() const;
    Types::VertData picking(const QVector3D& p, const QVector3D& n) const;
    const std::string& getName() const;

    //until we can get all the operation done directly in GPU - copy data to local memory
    bool updateVertexBuffer(const Types::VertData* data,size_t n) override;
    
};

