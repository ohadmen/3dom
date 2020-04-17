#include "track_state_idle.h"
#include "track_state_zoom.h"
#include "track_state_rotate.h"
#include "track_state_translate.h"
#include "track_state_retarget.h"
#include <cmath>
#include <sstream>
constexpr float inf = std::numeric_limits<float>::infinity();

TrackStateIdle::TrackStateIdle(TrackStateMachine* machine):TrackStateAbs(machine), m_mousepos(-1,-1),m_distanceMesurment{inf,inf,inf}{}


void TrackStateIdle::privSetDistanceMesuremnts()
{
    QVector3D x = m_machineP->pickClosestObject(m_mousepos);
	
    if (std::isinf(x[0]))
		return;
    
    if(std::isinf(m_distanceMesurment[0]))
        m_distanceMesurment=x;
    else
    {
           QVector3D d = x - m_distanceMesurment;
           std::stringstream ss;
           ss << "distance:"<< d.length() << " ["<<d.x() << ", " << d.y() << ", " << d.z() << "]";
           m_machineP->setStatus(ss.str());
           m_distanceMesurment[0]=inf;

    }

}

void TrackStateIdle::input(QKeyEvent* e)
{
    
    bool noModifier = e->modifiers() == Qt::KeyboardModifier::NoModifier;
    bool typeKeypress = e->type() == QInputEvent::KeyRelease;
    if(typeKeypress && noModifier && e->key() >= Qt::Key::Key_0 && e->key() <= Qt::Key::Key_9)
    {
        int k(e->key() - Qt::Key::Key_0);
        m_machineP->setTexture(k);
        m_machineP->canvasUpdate();
    }
    else if (typeKeypress && noModifier && e->key() == Qt::Key::Key_D)
    {
        privSetDistanceMesuremnts();
    }



}

void TrackStateIdle::input(QWheelEvent* e)
{
	if (e->type() == QInputEvent::Wheel && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		TrackStateZoom(m_machineP).input(e);
		

}
void TrackStateIdle::input(QMouseEvent* e)
{
    m_mousepos = e->localPos();
    qDebug() << "mouseMove";
	if (e->button() == Qt::MouseButton::LeftButton && e->type() == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
    {
		m_machineP->setState(new TrackStateRotate(m_machineP,m_mousepos));
    }
	else if (e->button() == Qt::MouseButton::RightButton && e->type() == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		m_machineP->setState(new TrackStateTranslate(m_machineP,m_mousepos));
	else if (e->button() == Qt::MouseButton::LeftButton && e->type() == QInputEvent::MouseButtonDblClick && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		TrackStateRetarget(m_machineP).input(m_mousepos);
	//else
	//	qDebug() << "type:" << e->type() << "button:" << e->button();

	
	
	


	

}
