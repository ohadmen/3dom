#include "state_machine.h"
#include "track_state_idle.h"
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

    for (const auto& o : DrawablesBuffer::i())
	{ 
        if (!o.second->isActive())
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

TrackStateMachine::TrackStateMachine() : m_state(new TrackStateIdle(this))
{
}
void TrackStateMachine::setState(TrackStateAbs *newState)
{
    delete m_state;
    m_state = newState;
    m_state->m_machineP = this;
    qDebug() << "current state:" << m_state->name();
}

void TrackStateMachine::input(QInputEvent *e)
{
    m_state->input(e);
}

void TrackStateMachine::setStatus(const std::string &str)
{
    emit signal_setStatus(str.c_str());
}
void TrackStateMachine::setTexture(int k)
{
    emit signal_setTexture(k);
}
void TrackStateMachine::canvasUpdate()
{
    emit signal_canvasUpdate();
}
QMatrix4x4 TrackStateMachine::getViewMatrix() const
{
    return m_vpmat.getViewMatrix();
}
void TrackStateMachine::setViewMatrix(const QMatrix4x4 &m)
{
    m_vpmat.setViewMatrix(m);
}

void 	   TrackStateMachine::setWinSize(int w, int h)
{
    m_vpmat.setWinSize(w,h);
}

QVector3D TrackStateMachine::xy2screen(const QPointF &xy)
{
    
    return m_vpmat.xy2screen(xy);
    
}
