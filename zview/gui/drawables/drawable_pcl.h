
#pragma once
#include "drawable_base.h"

class DrawablePcl : public Types::Pcl, public DrawableBase
{
	
	

public:
	Types::Shape getShape() const override;
	DrawablePcl(const std::string& name);
	~DrawablePcl();
	void initializeGL();
	void paintGL(const QMatrix4x4& mvp);
	Types::Roi3d get3dbbox() const;
	QVector3D picking(const QVector3D& p, const QVector3D& n) const;
    const std::string& getName() const;



};

