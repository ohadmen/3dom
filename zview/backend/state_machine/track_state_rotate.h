#pragma once
#include "track_state_abs.h"
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>

//----------------------state rotation----------------------//
class TrackStateRotate: public TrackStateAbs{
	QVector3D sprivGetHitSphere(const QPointF& xy);
	QVector3D m_hitonscreen;
	QMatrix4x4 m_hitview;
	
public:
	const char* name() override{ return "TrackStateRotate"; }
	TrackStateRotate(TrackStateMachine* machine);
	void setInitLocation(const QPointF& xy);

	void input(QMouseEvent* e);
};

