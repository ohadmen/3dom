#include "track_state_measure_distance.h"
#include "state_machine.h"
#include "zview/gui/drawables/drawables_buffer.h"
#include <cmath>
#include <sstream>
#include <iomanip>
void TrackStateMeasureDistance::privStateReset()
{
    constexpr float inf = std::numeric_limits<float>::infinity();
    m_p[0].x=inf;
    m_p[1].x=inf;
    if(m_drawObjHandle!=-1)
        drawablesBuffer.removeShape(m_drawObjHandle);
    m_drawObjHandle=-1;
}
TrackStateMeasureDistance::TrackStateMeasureDistance(TrackStateMachine *machineP) : TrackStateIdle(machineP),m_drawObjHandle(-1)
{
    privStateReset();
}
Types::VertData vecCol2vert(const QVector3D &v, const uint8_t *c)
{
    return {v.x(), v.y(), v.z(), c[0], c[1], c[2]};
}

Types::Edges makeEdgeShape(const Types::VertData &p0v, const Types::VertData &p1v)
{
    Types::Edges e(TrackStateMeasureDistance::measure_distance_object_name);
    e.v().push_back(p0v);
    e.v().push_back(p1v);
    e.e().push_back({0, 1});
    return e;
}
void TrackStateMeasureDistance::privSetMesuringEndPoint()
{

    QVector3D x = m_machineP->pickClosestObject(m_mousepos);
    if (std::isinf(x[0]))
        return;

    QVector3D d = (x - m_p[0]);
    std::stringstream ss;
    ss << std::setw(7) << std::setprecision(3) << "distance:" << d.length() << " [" << d.x() << ", " << d.y() << ", " << d.z() << "]";
    m_machineP->setStatus(ss.str());

    m_machineP->setCurrentState<TrackStateIdle>();
}

bool TrackStateMeasureDistance::setMesuringStartPoint(const QPointF &xy)
{
    privStateReset();
    constexpr uint8_t begcolor[] = {255, 0, 0};
    m_p[0] = vecCol2vert(m_machineP->pickClosestObject(xy), begcolor);
    if (std::isinf(m_p[0].x))
        return false;
    m_drawObjHandle = drawablesBuffer.addShape(makeEdgeShape(m_p[0], m_p[0]));
    m_machineP->canvasUpdate();

    return true;
}
void TrackStateMeasureDistance::input(QKeyEvent *e)
{
    bool noModifier = e->modifiers() == Qt::KeyboardModifier::NoModifier;
    bool typeKeypress = e->type() == QInputEvent::KeyRelease;
    if (typeKeypress && noModifier && e->key() == Qt::Key::Key_D)
    {
        privSetMesuringEndPoint();
    }
    else if (typeKeypress && noModifier && e->key() == Qt::Key::Key_Escape)
    {
        
        privStateReset();
         m_machineP->setCurrentState<TrackStateIdle>();
         m_machineP->canvasUpdate();


    }
    else
        TrackStateIdle::input(e);
}
void TrackStateMeasureDistance::input(QMouseEvent *e)
{
    TrackStateIdle::input(e);
    constexpr uint8_t endcolor[] = {255, 255, 0};
    QVector3D x = m_machineP->pickClosestObject(m_mousepos);
    if (std::isinf(x[0]))
    {
        drawablesBuffer.setShapeVisability(m_drawObjHandle,false);
        return;
    }
    m_p[1] = vecCol2vert(x, endcolor);
    drawablesBuffer.updateVertexBuffer(m_drawObjHandle, m_p,2);
    drawablesBuffer.setShapeVisability(m_drawObjHandle,true);
    
}
