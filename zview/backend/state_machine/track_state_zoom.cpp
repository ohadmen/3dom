#include "track_state_zoom.h"
#include "state_machine.h"
namespace
{
QMatrix4x4 scale(float v)
{
    QMatrix4x4 m;
    m.scale(v);
    return m;
}
QMatrix4x4 translate(const QVector3D &v)
{
    QMatrix4x4 m;
    m.translate(v);
    return m;
}

}

TrackStateZoom::TrackStateZoom(TrackStateMachine* p):TrackStateAbs(p) {}
void TrackStateZoom::input(QWheelEvent* e)
{
	static const float step = 1.1;
	
	QMatrix4x4 curvview = m_machineP->getViewMatrix();
	
	
	QMatrix4x4  m = translate(QVector3D(0, 0, 1));
	if (e->delta() > 0)
		m = scale(step)*m;
	else
		m = scale(1/step) * m;
	m = translate(QVector3D(0, 0, -1)) * m;
	m_machineP->setViewMatrix(m*curvview);
    m_machineP->canvasUpdate();

}
