#include "track_state_zoom.h"
#include "libcurie/common/vp_mat.h"

TrackStateZoom::TrackStateZoom(TrackStateMachine* p):TrackStateAbs(p) {}
void TrackStateZoom::input(QWheelEvent* e)
{
	static const float step = 1.1;
	
	QMatrix4x4 curvview = vpmat.getViewMatrix();
	
	
	QMatrix4x4  m = VPmat::translate(QVector3D(0, 0, 1));
	if (e->delta() > 0)
		m = VPmat::scale(step)*m;
	else
		m = VPmat::scale(1/step) * m;
	m = VPmat::translate(QVector3D(0, 0, -1)) * m;
	vpmat.setViewMatrix(m*curvview);
    canvasUpdate();

}
