#include "track_state_rotate.h"
#include "zview/common/params.h"
#include "state_machine.h"
#include <cassert>
#include <cmath>

//TrackStateRotate::ScreenHit TrackStateRotate::sprivGetHitSphere(const QPointF& xy)
QVector3D TrackStateRotate::sprivGetHitSphere(const QPointF &xy)
{
	/*
	hit area is represented by two function:
	za(x) = sqrt(r*r-x*x) 
	zb(x) = a/(x+b) -1
	the switch between the function happens on x0=alpha*r
	a and b are calculated s.t. zb(x) passes through (x0,z0) and that the derivative in this point is equal
	*/

	static const float switchP = 0.75;
	float tbradius2 = Params::trackBallRadius() * Params::trackBallRadius();
	float x0 = Params::trackBallRadius() * switchP;
	float y0 = std::sqrt(tbradius2 - x0 * x0);
	float parama = y0 * y0 * y0 / x0;
	float paramb = y0 * y0 / x0 - x0;

	QVector3D hitonscreen = m_machineP->xy2screen(xy);

	assert(tbradius2 < 1.0);
	float xyr2 = hitonscreen.x() * hitonscreen.x() + hitonscreen.y() * hitonscreen.y();
	float xyr = std::sqrt(xyr2);
	if (xyr >= x0)
		// use za(x)
		hitonscreen[2] = parama / (xyr + paramb) - 1;
	else
		// use za(x)
		hitonscreen[2] = sqrt(tbradius2 - xyr2) - 1;
	//substract (0,0,-1) to get the vector
	hitonscreen[2] += 1;
	return hitonscreen;
}

TrackStateRotate::TrackStateRotate(TrackStateMachine *machine) : TrackStateAbs(machine)
{
}
void TrackStateRotate::setInitLocation(const QPointF &xy)
{
	m_hitview = m_machineP->getViewMatrix();
	m_hitonscreen = sprivGetHitSphere(xy).normalized();
}
void TrackStateRotate::input(QMouseEvent *e)
{
	if (e->button() == Qt::MouseButton::NoButton && e->type() == QInputEvent::MouseMove && e->modifiers() == Qt::KeyboardModifier::NoModifier)
	{

		static const float rad2deg = 90.0 / std::acos(0.0);
		float tbradius = Params::trackBallRadius();
		QVector3D hitnew = sprivGetHitSphere(e->localPos());
		// when the scond hit is outside the sphere, scale by the length of the vector
		float angleScale = hitnew.length() / tbradius;
		hitnew.normalize();
		QVector3D axis = QVector3D::crossProduct(hitnew, m_hitonscreen);
		float phi = -std::acos(QVector3D::dotProduct(hitnew, m_hitonscreen)) * rad2deg * angleScale;
		QMatrix4x4 m;
		m.rotate(phi, axis);
		m(0, 3) = m(0, 2);
		m(1, 3) = m(1, 2);
		m(2, 3) = m(2, 2) - 1;
		m_machineP->setViewMatrix(m * m_hitview);
		m_machineP->canvasUpdate();
	}
	if (e->button() == Qt::MouseButton::LeftButton && e->type() == QInputEvent::MouseButtonRelease && e->modifiers() == Qt::KeyboardModifier::NoModifier)
		m_machineP->returnToPerviousState();
}
