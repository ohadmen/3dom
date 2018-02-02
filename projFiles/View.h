#pragma once
#include <QMatrix4x4>
#include <Q3d\QLine3D.h>
#include <array>


class View {
	QMatrix4x4 proj;
	QMatrix4x4 model;
	QMatrix4x4 matrix;
	QMatrix4x4 inverse;
	int viewport[4];
	bool m_isOrtho;
	
public:
	View() :m_isOrtho(false) {}

	bool isOrtho() const { return m_isOrtho; }
	void getView() {
		glGetFloatv(GL_PROJECTION_MATRIX, proj.data());
		glGetFloatv(GL_MODELVIEW_MATRIX, model.data());
		glGetIntegerv(GL_VIEWPORT, (GLint*)viewport);

		if (proj(3,3) == 0) m_isOrtho = false;
		else m_isOrtho = true;

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

		return  model.inverted()*QVector3D(0, 0, m_isOrtho ? 3 : 0);
	}
	

//
//	// Note that p it is assumed to be in model coordinate.
//	QPlane3D viewPlaneFromModel(const QVector3D& p)
//	{
//		//compute normal, pointing away from view.
//		QMatrix4x4 imodel = model.inverted();
//		QVector3D vp = viewPoint();
//		QVector3D n = imodel * QVector3D(0.0f, 0, -1.0f) - vp;
//		
//		
//		
//		return QPlane3D(p,n);
//	}
//
	// Note that p it is assumed to be in model coordinate.
	QRay3D viewRayFromModel(const QVector3D &p)
	{
		
		QVector3D vp = viewPoint();
		return QRay3D(m_isOrtho ? p : vp, m_isOrtho ? -vp : (p - vp));
	
	}

	// Note that p it is assumed to be in window coordinate.
	QRay3D viewRayFromWindow(const QVector3D &p)
	{
		QVector3D vp = viewPoint();
		QVector3D pp = unProject(p);
		return QRay3D(m_isOrtho ? pp : vp, m_isOrtho ? -vp : (pp - vp));
		
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

	// Note that p it is assumed to be in model coordinate.
	QLine3D viewLineFromModel(const QVector3D &p) const
	{
		QLine3D line;
		QVector3D vp = viewPoint();
		if (m_isOrtho) {
			line = QLine3D(p, -vp + p);
			//line.setOrigin(p);
			//line.direction(-vp);
		}
		else {
			line = QLine3D(vp, p);
			//line.SetOrigin(vp);
			//line.SetDirection(p - vp);
		}
		return line;
	}

	// Note that p it is assumed to be in window coordinate.
	QLine3D viewLineFromWindow(const QVector3D &p) const
	{
		QLine3D line;  // plane perpedicular to view direction and passing through manip center
		QVector3D vp = viewPoint();
		QVector3D pp = unProject(p);

		if (m_isOrtho) {
			line = QLine3D(pp, -vp + pp);
			//line.SetOrigin(pp);
			//line.SetDirection(-vp);
		}
		else {
			line = QLine3D(vp,  pp);
			//line.SetOrigin(vp);
			//line.SetDirection(pp - vp);
		}
		return line;
	}
};