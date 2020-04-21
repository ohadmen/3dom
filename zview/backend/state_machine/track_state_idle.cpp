#include "track_state_idle.h"
#include "track_state_zoom.h"
#include "track_state_rotate.h"
#include "track_state_translate.h"
#include "track_state_retarget.h"
#include "track_state_measure_distance.h"
#include "state_machine.h"
#include <cmath>
#include <sstream>
constexpr float inf = std::numeric_limits<float>::infinity();

TrackStateIdle::TrackStateIdle(TrackStateMachine* machine):TrackStateAbs(machine), m_mousepos(-1,-1){}


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
        
        TrackStateMeasureDistance* tsmd = m_machineP->getState<TrackStateMeasureDistance>();
               
        if (tsmd->setMesuringStartPoint(m_mousepos))
            m_machineP->setCurrentState<TrackStateMeasureDistance>();
            
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
    {
		m_machineP->setCurrentState<TrackStateRotate>();
        m_machineP->getState<TrackStateRotate>()->setInitLocation(m_mousepos);
    }
	else if (e->button() == Qt::MouseButton::RightButton && e->type() == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
    {
		m_machineP->setCurrentState<TrackStateTranslate>();
        m_machineP->getState<TrackStateTranslate>()->setInitLocation(m_mousepos);
    }
	else if (e->button() == Qt::MouseButton::LeftButton && e->type() == QInputEvent::MouseButtonDblClick && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		m_machineP->getState<TrackStateRetarget>()->input(m_mousepos);

	
	
	


	

}
