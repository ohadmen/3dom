#pragma once
#include "track_state_machine.h"
#include <QtCore/QEvent>
//----------------------State base class----------------------//
class TrackStateAbs 
{
	
	friend TrackStateMachine;
protected:
	TrackStateMachine* m_machineP;
public:
	virtual const char* name() = 0;
	
	void input(QInputEvent* e);
	void canvasUpdate();
    
    void setStatus(const std::string& str);

	virtual void input(QMouseEvent*) { return; }
	virtual void input(QKeyEvent*) { return; }
	virtual void input(QWheelEvent*) { return; }
    TrackStateAbs(TrackStateMachine* machine = nullptr) :m_machineP(machine) {}

};



