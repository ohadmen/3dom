#pragma once
#include "track_state_abs.h"
//----------------------state retarget----------------------//
class TrackStateRetarget : public TrackStateAbs {


public:
    const char* name() { return "TrackStateRetarget"; }
    void input(const QPointF& xy);
    TrackStateRetarget(TrackStateMachine* machine);

};

