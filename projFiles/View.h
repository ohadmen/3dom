#pragma once
#include <QMatrix4x4>
#include <QtOpenGL/QGLFunctions>
#include <array>

#include "GeomObjs.h"
class View {
	QMatrix4x4 proj;
	QMatrix4x4 model;
	QMatrix4x4 matrix;
	QMatrix4x4 inverse;
	int viewport[4];
	bool isOrtho;
	
public:
	View() :isOrtho(false) {}

	void getView() {
		glGetFloatv(GL_PROJECTION_MATRIX, proj.data());
		glGetFloatv(GL_MODELVIEW_MATRIX, model.data());
		glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);

		if (proj(3,3) == 0) isOrtho = false;
		else isOrtho = true;

		matrix = proj*model;
		inverse = matrix.inverted();
	}
	void setView(const float *_proj,		const float *_modelview,		const int *_viewport) {
		for (int i = 0; i < 4; i++) {
			for (int k = 0; k < 4; k++) {
				proj(i,k) = _proj[4 * i + k];
				model(i,k) = _modelview[4 * i + k];
			}
			viewport[i] = _viewport[i];
		}
		matrix = proj*model;
		inverse = matrix.inverted();
		
	}
	QVector3D viewPoint() const {

		return  model.inverted()*QVector3D(0, 0, isOrtho ? 3 : 0);
	}
	


	// Note that p it is assumed to be in model coordinate.
	Plane viewPlaneFromModel(const QVector3D& p)
	{
		//compute normal, pointing away from view.
		QMatrix4x4 imodel = model.inverted();
		QVector3D vp = viewPoint();
		QVector3D n = imodel * QVector3D(0.0f, 0, -1.0f) - vp;

		
		
		return Plane(p,n);
	}

	// Note that p it is assumed to be in model coordinate.
	Ray viewRayFromModel(const QVector3D &p)
	{
		
		QVector3D vp = viewPoint();
		return Ray(isOrtho ? p : vp, isOrtho ? -vp : (p - vp));
	
	}

	// Note that p it is assumed to be in window coordinate.
	Ray viewRayFromWindow(const QVector3D &p)
	{
		QVector3D vp = viewPoint();
		QVector3D pp = unProject(p);
		return Ray(isOrtho ? pp : vp, isOrtho ? -vp : (pp - vp));
		
	}

	QVector3D project(const QVector3D &p) const {
		QVector3D r;
		r = matrix * p;
		return normDevCoordToWindowCoord(r);
	}

	QVector3D unProject(const QVector3D &p) const {
		QVector3D s = windowCoordToNormDevCoord(p);
		s = inverse * s;
		return s;
	}


	QVector3D normDevCoordToWindowCoord(const QVector3D &p) const {
		QVector3D a;
		a[0] = (p[0] + 1)*(viewport[2] / 2.0f) + viewport[0];
		a[1] = (p[1] + 1)*(viewport[3] / 2.0f) + viewport[1];
		//a[1] = viewport[3] - a[1];
		a[2] = (p[2] + 1) / 2;
		return a;
	}


	QVector3D windowCoordToNormDevCoord(const QVector3D &p) const {
		QVector3D a;
		a[0] = (p[0] - viewport[0]) / (viewport[2] / 2.0f) - 1;
		a[1] = (p[1] - viewport[1]) / (viewport[3] / 2.0f) - 1;
		//a[1] = -a[1];
		a[2] = 2 * p[2] - 1;
		return a;
	}

};