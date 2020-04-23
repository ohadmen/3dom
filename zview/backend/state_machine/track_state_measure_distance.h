#pragma once
#include "track_state_idle.h"
#include "zview/common/common_types.h"
#include <QtGui/QVector3D>
//----------------------state zoom----------------------//
class TrackStateMeasureDistance : public TrackStateIdle {
    static constexpr char drawble_p0_key[] = "TrackStateMeasureDistance_p0";
    static constexpr char drawble_p1_key[] = "TrackStateMeasureDistance_p1";
    static constexpr char drawble_line_key[] = "TrackStateMeasureDistance_line";

    Types::VertData m_p[2];;
    int m_drawObjHandle;
    void privSetMesuringEndPoint();
    void privStateReset();
    
public:
    static constexpr char measure_distance_object_name[] = "distance measurment";
    bool setMesuringStartPoint(const QPointF&);
	const char* name() { return "TrackStateMeasureDistance"; }
    TrackStateMeasureDistance(TrackStateMachine* machineP);
    void input(QKeyEvent* e) override;
    void input(QMouseEvent* e);
    
	
};



