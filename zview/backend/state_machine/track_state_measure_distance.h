#pragma once
#include "track_state_idle.h"
#include <QtGui/QVector3D>
//----------------------state zoom----------------------//
class TrackStateMeasureDistance : public TrackStateIdle {
    static constexpr char drawble_p0_key[] = "TrackStateMeasureDistance_p0";
    static constexpr char drawble_p1_key[] = "TrackStateMeasureDistance_p1";
    static constexpr char drawble_line_key[] = "TrackStateMeasureDistance_line";

    QVector3D m_p0;
public:
    bool setMesuringStartPoint(const QPointF&);
	const char* name() { return "TrackStateMeasureDistance"; }
    TrackStateMeasureDistance(TrackStateMachine* machine);
    void input(QKeyEvent*) override;
    
	
};



