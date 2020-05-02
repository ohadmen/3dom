
#include "state_machine.h"
#include "track_state_idle.h"
#include "track_state_measure_distance.h"

#include "zview/gui/drawables/drawables_buffer.h"

#include <sstream>

QVector3D TrackStateMachine::pickClosestObject(const QPointF &xy) const
{
    static const float inf = std::numeric_limits<float>::infinity();
    QMatrix4x4 mat = getViewMatrix();

    auto ray = m_vpmat.xy2ray(xy);
    QVector2D pcam(mat * (ray.first + ray.second));
    float minLen = inf;
    QVector3D pt(inf, inf, inf);

    for (const auto &o : drawablesBuffer)
    {
        if (!o.second->isActive())
            continue;
        if(o.second->getName()==TrackStateMeasureDistance::measure_distance_object_name)
            continue;
        QVector3D x = o.second.get()->picking(ray.first, ray.second);
        QVector2D xcam(mat * x);
        float xcamLen = (xcam - pcam).length();
        if (minLen > xcamLen)
        {
            minLen = xcamLen;
            pt = x;
        }
    }
    return pt;
}

TrackStateMachine::TrackStateMachine()
{
    setCurrentState<TrackStateIdle>();
}
void TrackStateMachine::updatePmat()
{
    m_vpmat.updatePmat();
}

void TrackStateMachine::input(QInputEvent *e)
{
    m_statesMap[m_currentState]->input(e);
}

void TrackStateMachine::setStatus(const std::string &str)
{
    emit signal_setStatus(str.c_str());
}
void TrackStateMachine::canvasUpdate()
{
    emit signal_canvasUpdate();
}
QMatrix4x4 TrackStateMachine::getViewMatrix() const
{
    return m_vpmat.getViewMatrix();
}
QMatrix4x4 TrackStateMachine::getVPmatrix() const
{
    return m_vpmat.getVPmatrix();
}
void TrackStateMachine::setViewMatrix(const QMatrix4x4 &m)
{
    m_vpmat.setViewMatrix(m);
}

void TrackStateMachine::setWinSize(int w, int h)
{
    m_vpmat.setWinSize(w, h);
}

QVector3D TrackStateMachine::xy2screen(const QPointF &xy)
{

    return m_vpmat.xy2screen(xy);
}
