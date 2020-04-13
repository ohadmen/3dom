#include "track_state_translate.h"
#include "track_state_idle.h"

TrackStateTranslate::TrackStateTranslate(const QPointF& xy)
{
	m_hitview = m_machineP->getViewMatrix();
	
	m_hitonscreen = m_machineP->xy2screen(xy);
}
void TrackStateTranslate::input(QMouseEvent* e)
{	
	if (e->button() == Qt::MouseButton::NoButton && e->type() == QInputEvent::MouseMove && e->modifiers() == Qt::KeyboardModifier::NoModifier)
	{
		QVector3D d = m_machineP->xy2screen(e->localPos())- m_hitonscreen;
		QMatrix4x4 t; t.translate(d);
		m_machineP->setViewMatrix(t * m_hitview);
        m_machineP->canvasUpdate();
	}

	if (e->button() == Qt::MouseButton::RightButton && e->type() == QInputEvent::MouseButtonRelease && e->modifiers() == Qt::KeyboardModifier::NoModifier)
	{
		m_machineP->setState(new TrackStateIdle);
	}
}


