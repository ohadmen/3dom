#pragma once
#include "track_state_abs.h"
//----------------------state idle----------------------//
class TrackStateIdle : public TrackStateAbs {
    QPointF m_mousepos;
    QVector3D m_distanceMesurment;
    void privSetDistanceMesuremnts();
protected:
    
public:
	const char* name() { return "TrackStateIdle"; }
    TrackStateIdle(TrackStateMachine* machine);
	
    void input(QKeyEvent* e);
    void input(QMouseEvent* e);
	void input(QWheelEvent* e);
	
};
