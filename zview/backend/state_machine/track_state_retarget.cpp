#include "track_state_retarget.h"
#include <QtGui/QVector3D>
#include <QtGui/QMatrix4x4>
#include <cmath>
#include <sstream>
#include <iomanip>

std::string strginFromQVector3d(const QVector3D& x)
{
    std::stringstream ss;
    ss <<"[" << std::setw(7) << std::setprecision(3) << x.x() << ", " << x.y() << ", " << x.z() << "]";
    return ss.str();

}

TrackStateRetarget::TrackStateRetarget(TrackStateMachine* machine) :TrackStateAbs(machine) {}
void TrackStateRetarget::input(const QPointF& xy)
{
    QVector3D x = m_machineP->pickClosestObject(xy);
	if (std::isinf(x[0]))
		return;
	//drawableBasicShapes.insert({ p,p + n * 100 }, "view ray", { 0,1,0 });
	//drawableBasicShapes.insert(x, "hit", { 1,0,0 });
	
	//vm moves from world to camera. vmI moves from camera to world.
	//the translation part in vmI represents the center of the shape, update it according to the
	//new retarget center.
	//after inversing back, reduce -1 from z as we put our shape infron of the camera at (0,0,-1)
    QMatrix4x4 vmI = m_machineP->getViewMatrix().inverted();
	for(int i=0;i!=3;++i)
		vmI(i,3) = x[i];
	QMatrix4x4 vm_new = vmI.inverted();
	vm_new(2, 3) -= 1;
	m_machineP->setViewMatrix(vm_new);
    m_machineP->canvasUpdate();

    m_machineP->setStatus(strginFromQVector3d(x));
    
}
