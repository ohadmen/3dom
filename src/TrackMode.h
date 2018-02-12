#ifndef TRACKMODE_H
#define TRACKMODE_H
#include <QVector2D>
#include "Qmvp.h"
#include "TrackUtils.h"
class TrackMode {
public:
	struct SharedResConst
	{
		const Qmvp& mvp;
		const Qmvp& mvpPrev;
		const QVector2D& mouseClickPrev;
	};
protected:
	const SharedResConst& m_sr;
	TrackUtils m_tu;
public:
	TrackMode(const SharedResConst& sharedRes):m_sr(sharedRes){}

	virtual ~TrackMode() {
	}

	virtual Qmvp apply(const QVector2D& newPoint) { return Qmvp(); }
	
	virtual Qmvp apply(float WheelNotch) { return Qmvp(); }

	virtual void setAction() {}

	virtual void reset() {}

	virtual const char* name() {return "TrackMode";	};

	virtual void draw() { m_tu.drawSphereIcon(m_sr.mvp.getMat(), true); }

};

class InactiveMode :public TrackMode {
public:
	const char *Name() {return "InactiveMode";	};

	void draw()
	{
		m_tu.drawSphereIcon(m_sr.mvp.getMat(),false);
	}
};
class SphereMode :public TrackMode {
public:
	const char *name() { return "SphereMode"; };

	virtual Qmvp apply(const QVector2D& newPoint)
	{
		QVector3D hitOld = m_tu.hitSphere(tb, tb.getLastPoint());
		QVector3D hitNew = m_tu.hitSphere(tb, new_point);

		QVector3D axis = QVector3D::crossProduct(hitNew, hitOld);
		axis.normalize();


		//  Figure out how much to rotate around that axis.
		//  float phi = Distance (hitNew, hitOld) / tb->radius;
		//  float phi = vcg::Angle(hitNew - center,hitOld - center)*(Distance(hitNew,center)/tb->radius);
		float phi = std::max(TrackUtils::vvAngle(hitNew, hitOld), ((hitNew, hitOld).length() / tb.getRadius()));
		

		Qmvp inc;
		inc.setR(axis,-phi);
		return inc;
	}

	
};



#endif
