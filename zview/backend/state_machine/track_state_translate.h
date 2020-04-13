#pragma once
#include "track_state_abs.h"



//----------------------state translation----------------------//
class TrackStateTranslate : public TrackStateAbs {
	QVector3D m_hitonscreen;
	QMatrix4x4 m_hitview;

public:
	const char* name() { return "TrackStateTranslate"; }
	TrackStateTranslate(const QPointF& xy);
	void input(QMouseEvent* e);
};


