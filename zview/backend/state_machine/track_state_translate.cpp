#include "track_state_translate.h"
#include "track_state_idle.h"
#include "libcurie/common/vp_mat.h"

TrackStateTranslate::TrackStateTranslate(const QPointF& xy)
{
	m_hitview = vpmat.getViewMatrix();
	m_hitonscreen = vpmat.xy2screen(xy);
}
void TrackStateTranslate::input(QMouseEvent* e)
{	
	if (e->button() == Qt::MouseButton::NoButton && e->type() == QInputEvent::MouseMove && e->modifiers() == Qt::KeyboardModifier::NoModifier)
	{
		QVector3D d = vpmat.xy2screen(e->localPos())- m_hitonscreen;
		QMatrix4x4 t; t.translate(d);
		vpmat.setViewMatrix(t * m_hitview);
        canvasUpdate();
	}

	if (e->button() == Qt::MouseButton::RightButton && e->type() == QInputEvent::MouseButtonRelease && e->modifiers() == Qt::KeyboardModifier::NoModifier)
	{
		m_machineP->setState(new TrackStateIdle);
	}
}


