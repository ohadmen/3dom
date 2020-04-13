#include "track_state_idle.h"
#include "track_state_zoom.h"
#include "track_state_rotate.h"
#include "track_state_translate.h"
#include "track_state_retarget.h"
#include "track_state_measure_distance.h"

TrackStateIdle::TrackStateIdle(TrackStateMachine* machine):TrackStateAbs(machine), m_mousepos(-1,-1){}

void TrackStateIdle::input(QKeyEvent* e)
{
    if(e->type() == QInputEvent::KeyPress && e->modifiers() == Qt::KeyboardModifier::NoModifier && e->key() >= Qt::Key::Key_0 && e->key() <= Qt::Key::Key_9)
    {
        int k(e->key() - Qt::Key::Key_0);
        m_machineP->setTexture(k);
        m_machineP->canvasUpdate();
    }
    else if (e->type() == QInputEvent::KeyPress && e->modifiers() == Qt::KeyboardModifier::NoModifier && e->key() == Qt::Key::Key_D)
    {
        TrackStateMeasureDistance* tsmd = new TrackStateMeasureDistance;
        if (tsmd->setMesuringStartPoint(m_mousepos))
            m_machineP->setState(tsmd);
        else
            delete tsmd;
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
	if (e->button() == Qt::MouseButton::LeftButton && e->type() == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		m_machineP->setState(new TrackStateRotate(m_mousepos));
	else if (e->button() == Qt::MouseButton::RightButton && e->type() == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		m_machineP->setState(new TrackStateTranslate(m_mousepos));
	else if (e->button() == Qt::MouseButton::LeftButton && e->type() == QInputEvent::MouseButtonDblClick && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		TrackStateRetarget(m_machineP).input(m_mousepos);
	//else
	//	qDebug() << "type:" << e->type() << "button:" << e->button();

	
	
	


	

}
