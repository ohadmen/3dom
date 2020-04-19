#pragma once
#include "track_state_abs.h"

//----------------------state zoom----------------------//
class TrackStateZoom : public TrackStateAbs {


public:
	const char* name()override { return "TrackStateZoom"; }
    TrackStateZoom(TrackStateMachine* p);
	void input(QWheelEvent*);
	
};



