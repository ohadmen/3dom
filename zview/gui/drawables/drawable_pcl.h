
#pragma once
#include "drawable_base.h"

class DrawablePcl : public Types::Pcl, public DrawableBase
{
	
	

public:
	DrawablePcl(const std::string& name);
	~DrawablePcl();
	void initializeGL();
	void paintGL(const QMatrix4x4& mvp, int txt);
	Types::Roi3d get3dbbox() const;
	QVector3D picking(const QVector3D& p, const QVector3D& n) const;
    



};

