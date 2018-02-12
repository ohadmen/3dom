
#ifndef QMVP
#define  QVMP
#include <QMatrix4x4>
#include <qquaternion.h>
#include "Q3d/QPlane3D.h"

class Qmvp
{
	//QMatrix4x4 m_model; //UNUSED
	QQuaternion m_viewR;
	QVector3D   m_viewT;
	float		m_viewS;
	QMatrix4x4 m_proj;


	QMatrix4x4 pR()const{ QMatrix4x4 r; r.rotate(m_viewR);   return std::move(r);}
	QMatrix4x4 pS()const{ QMatrix4x4 s; s.scale(m_viewS);	return std::move(s);}
	QMatrix4x4 pT()const{ QMatrix4x4 t; t.translate(m_viewT);return std::move(t);}

	QMatrix4x4 pRi()const { QMatrix4x4 r; r.rotate(m_viewR.inverted());   return std::move(r); }
	QMatrix4x4 pSi()const { QMatrix4x4 s; s.scale(1 / m_viewS);	return std::move(s); }
	QMatrix4x4 pTi()const { QMatrix4x4 t; t.translate(-m_viewT); return std::move(t); }

	QMatrix4x4 pV()const {	return pT()*pS()*pR();	}
	QMatrix4x4 pVi()const { return pRi()*pSi()*pTi(); }

public:
	Qmvp() {
		
		resetView();
	}
	void resetView()
	{
		setPerpective(1.0);
		m_viewR = QQuaternion(0, QVector3D(0, 0, 1));
		m_viewT = QVector3D(0, 0, 0);
		m_viewS = 1.0f;
	}
	//QMatrix4x4 getMmat()const { return m_proj; }
	QMatrix4x4 getPmat()const { return m_proj; }
	//QMatrix4x4 getVmat()const {  }

	void applyT(const QVector3D& t) { m_viewT += t; }
	void applyR(const QVector3D& axis, float angle)
	{
		m_viewR = QQuaternion::fromAxisAndAngle(axis, angle) * m_viewR;
	}

	void setPerpective(float aspectratio)
	{
		m_proj.setToIdentity();
		m_proj.perspective(Params::camFOV(), aspectratio, Params::camZnear(), Params::camZfar());
	}
	QMatrix4x4 getMat() const
	{
			//return m_proj*s*r*t;  // trans * scale * rot;
			return m_proj*pV();  // trans * scale * rot;
	}
	QMatrix4x4 getMatNoScale() const
	{
		return m_proj*pT()*pR();
	}
	QMatrix4x4 getMatInv() const
	{
		//return m_proj*s*r*t;  // trans * scale * rot;
		return pVi()*m_proj.inverted();  // trans * scale * rot;
	}


	bool isOrth() const { return m_proj(3, 3) == 0; }
	QVector3D viewPoint() const {

		return  pVi()*QVector3D(0, 0, isOrth()? 3 : 0);
	}
	// Note that p it is assumed to be in model coordinate.
	QPlane3D viewPlaneFromModel(const QVector3D& p)
	{

		QVector3D n = pVi() * QVector3D(0, 0, -1);
		return QPlane3D(p, n);
	}

	QRay3D viewRayFromModel(const QVector3D &p)
	{

		QVector3D vp = viewPoint();
		return QRay3D(isOrth() ? p : vp, isOrth() ? -vp : (p - vp));

	}

	// Note that p it is assumed to be in window coordinate.
	QRay3D viewRayFromWindow(const QVector3D &p)
	{
		QVector3D vp = viewPoint();
		QVector3D pp = unProject(p);
		return QRay3D(isOrth() ? pp : vp, isOrth() ? -vp : (pp - vp));

	}

	QVector3D project(const QVector3D &p) const {
		QVector3D r;
		r = getMat() * p;
		return normDevCoordToWindowCoord(r);
	}

	QVector3D unProject(const QVector3D &p) const {
		QVector3D s = windowCoordToNormDevCoord(p);
		s = getMatInv() * s;
		return s;
	}


	QVector3D normDevCoordToWindowCoord(const QVector3D &p) const {
		//QVector3D a;
		//a[0] = (p[0] + 1)*(viewport[2] / 2.0f) + viewport[0];
		//a[1] = (p[1] + 1)*(viewport[3] / 2.0f) + viewport[1];
		//a[2] = (p[2] + 1) / 2;
		//return a;
		return m_proj*p;
	}


	QVector3D windowCoordToNormDevCoord(const QVector3D &p) const {
		//QVector3D a;
		//a[0] = (p[0] - viewport[0]) / (viewport[2] / 2.0f) - 1;
		//a[1] = (p[1] - viewport[1]) / (viewport[3] / 2.0f) - 1;
		////a[1] = -a[1];
		//a[2] = 2 * p[2] - 1;
		//return a;
		return m_proj.inverted()*p;
	}

	// Note that p it is assumed to be in model coordinate.
	QLine3D viewLineFromModel(const QVector3D &p) const
	{
		QLine3D line;
		QVector3D vp = viewPoint();
		if (isOrth()) {
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

		if (isOrth()) {
			line = QLine3D(pp, -vp + pp);
			//line.SetOrigin(pp);
			//line.SetDirection(-vp);
		}
		else {
			line = QLine3D(vp, pp);
			//line.SetOrigin(vp);
			//line.SetDirection(pp - vp);
		}
		return line;
	}

};
#endif
