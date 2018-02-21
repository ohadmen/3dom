#pragma  once
#include <QMatrix4x4>
#include <qquaternion.h>
#include "Q3d/QPlane3D.h"
#include "Params.h"

class Qmvp
{
	//QQuaternion m_modelR;
	//QVector3D   m_modelT;
	//float		m_modelS;


	QQuaternion m_viewR;
	QVector3D   m_viewT;
	float		m_viewS;

	QMatrix4x4 m_proj;
	QVector2D m_uv2pix;//[w h]/2


	
	QMatrix4x4 pV()const {	return getT()*getS()*getR();	}
	QMatrix4x4 pVi()const { return getRi()*getSi()*getTi(); }

public:
	Qmvp() {
		resetView(1, 1);
		
		m_uv2pix=QVector2D(1,1);
		
	}
	void resetView(int w, int h)
	{
		setWinSize(w,h);
		m_viewR = QQuaternion(0, QVector3D(0, 0, 1));
		m_viewT = QVector3D(0, 0, 0);
		m_viewS = 1.0f;
	}

	void applyT(const QVector3D& t) { m_viewT += t; }
	void applyR(const QVector3D& axis, float angle)	{		m_viewR = QQuaternion::fromAxisAndAngle(axis, angle) * m_viewR;	}
	void applyS(float s) { m_viewS *= s; }

	QMatrix4x4 getR()const  { QMatrix4x4 r;r.setToIdentity();r.rotate(m_viewR);   return std::move(r); }
	QMatrix4x4 getS()const  { QMatrix4x4 s;s.setToIdentity();s.scale(m_viewS);	return std::move(s); }
	QMatrix4x4 getT()const  { QMatrix4x4 t;t.setToIdentity();t.translate(m_viewT); return std::move(t); }

	void setR(const QQuaternion& r) { m_viewR = r; }
	void setS(float s) { m_viewS = s; }
	void setT(const QVector3D& t) { m_viewT = t; }
	QMatrix4x4 getRi()const { QMatrix4x4 r;r.setToIdentity();r.rotate(m_viewR.inverted());   return std::move(r); }
	QMatrix4x4 getSi()const { QMatrix4x4 s;s.setToIdentity();s.scale(1 / m_viewS);	return std::move(s); }
	QMatrix4x4 getTi()const { QMatrix4x4 t;t.setToIdentity();t.translate(-m_viewT); return std::move(t); }
	QMatrix4x4 getP()const { return m_proj; }
	QMatrix4x4 getPi()const { return m_proj.inverted(); }

	void setWinSize(int w, int h)
	{
		float ar = float(w) / float(h ? h : 1);
		m_uv2pix = QVector2D(w / 2.0, h / 2.0);
		m_proj.setToIdentity();
		m_proj.perspective(Params::camFOV()/ar, ar, Params::camZnear(), Params::camZfar());
	}
	float getAspectRatio() const 
	{
		return m_proj(1, 1) / m_proj(0, 0);
	}
	QMatrix4x4 getMat() const
	{
			//return m_proj*s*r*t;  // trans * scale * rot;
			return m_proj*pV();  // trans * scale * rot;
	}
	
	QMatrix4x4 getMatInv() const
	{
		//return m_proj*s*r*t;  // trans * scale * rot;
		return pVi()*m_proj.inverted();  // trans * scale * rot;
	}


	bool isOrth() const { return m_proj(3, 3) == 0; }
	QVector3D getViewPoint() const { 		return getPi()*QVector3D(0, 0, isOrth()? 3 : 0);	}
	QPlane3D  getViewPlane() const { return QPlane3D(getViewPoint()- m_viewT, 1); }

	QPlane3D viewPlaneFromModel(const QVector3D& p)
	{
	
		QVector3D n = pVi() * QVector3D(0, 0, -1);
		return QPlane3D(p, n);
	}

	QRay3D viewRayFromModel(const QVector3D &p)
	{
	
		QVector3D vp = getViewPoint();
		return QRay3D(isOrth() ? p : vp, isOrth() ? -vp : (p - vp));
	
	}

	 
	QRay3D viewRayFromWindow(const QVector2D &p)
	{
		QVector3D vp = getViewPoint();
		QVector3D pp = unProject(p);
		return QRay3D(isOrth() ? pp : vp, isOrth() ? -vp : (pp - vp));
	
	}

	QVector2D project(const QVector3D &xyz) const {
		QVector3D uv = getMat() * xyz; //[-1 1]
		QVector2D pix = QVector2D(uv[0] / uv[2], uv[1] / uv[2])*m_uv2pix + m_uv2pix;;
		
		return  pix;
	}

	QVector2D pix2uv(const QVector2D &pix) const {
		QVector2D uv = (pix - m_uv2pix) / m_uv2pix;
		uv[1] *= -1;//inverse Y
		return uv;
	}

	QVector3D unProject(const QVector2D &pix) const {
		QVector2D uv = pix2uv(pix);


		QVector3D xyz = getPi() * QVector3D(uv, 1);
		xyz /= -xyz[2];
		return xyz;
	}


	QLine3D viewLineFromModel(const QVector3D &p) const
	{
		QLine3D line;
		QVector3D vp = getViewPoint();
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

	
	
	QLine3D viewLineFromWindow(const QVector2D& p) const
	{
		QLine3D line;  // plane perpedicular to view direction and passing through manip center
		QVector3D vp = getViewPoint();
		QVector3D pp = unProject(p);
	
		line = QLine3D(vp, pp);
		
		return line;
	}

};

//Qmvp operator*(const Qmvp& a, const Qmvp& b)
//{
//	Qmvp c;
//	c.m_viewR  = a.m_viewR  * b.m_viewR	;
//	c.m_viewT  = a.m_viewT  * b.m_viewT	;
//	c.m_viewS  = a.m_viewS  * b.m_viewS	;
//	c.m_proj   = a.m_proj   * b.m_proj	;
//	c.m_uv2pix = a.m_uv2pix * b.m_uv2pix;
//}

