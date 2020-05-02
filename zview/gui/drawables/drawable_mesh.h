#pragma once
#include "drawable_base.h"
#include "QtGui/QVector2D"

class DrawableMesh : public Types::Mesh, public DrawableBase
{

    QOpenGLBuffer m_iBuff; //indices 3xm
    
public:
Types::Shape getShape() const override;
	DrawableMesh(const std::string& name);
	~DrawableMesh();
	void initializeGL();
	void paintGL(const QMatrix4x4& mvp);
    QVector3D picking(const QVector3D& p, const QVector3D& n) const;
	Types::Roi3d get3dbbox()const;
	const std::string& getName() const;

};

