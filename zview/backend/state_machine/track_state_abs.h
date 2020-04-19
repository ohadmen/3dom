#pragma once
#include "QtGui/qmatrix4x4.h"
#include <QtGui/QInputEvent>
//----------------------State base class----------------------//
class TrackStateMachine;
class TrackStateAbs 
{
	
	friend TrackStateMachine;
protected:
	TrackStateMachine* m_machineP;
public:
	~TrackStateAbs();
	virtual const char* name() = 0;
	
	void input(QInputEvent* e);

	
	virtual void input(QMouseEvent*) { return; }
	virtual void input(QKeyEvent*) { return; }
	virtual void input(QWheelEvent*) { return; }
    TrackStateAbs(TrackStateMachine* machine) :m_machineP(machine) {}



};



