#pragma once
#include "track_state_abs.h"
//----------------------state idle----------------------//
class TrackStateIdle : public TrackStateAbs {
protected:
    QPointF m_mousepos;
public:
	const char* name() { return "TrackStateIdle"; }
    TrackStateIdle(TrackStateMachine* p = nullptr);
	
    void input(QKeyEvent* e);
    void input(QMouseEvent* e);
	void input(QWheelEvent* e);
	
};
