#include "track_state_abs.h"
#include <sstream>
#include <QtCore/QDebug>
#include <QtGui/QVector3D>
void TrackStateAbs::canvasUpdate()
{
	emit m_machineP->canvasUpdate();
}

void TrackStateAbs::input(QInputEvent* e)
{
	switch (e->type())
	{
	case QInputEvent::MouseButtonPress:
	case QInputEvent::MouseButtonRelease:
	case QInputEvent::MouseButtonDblClick:
	case QInputEvent::MouseMove:
	case QInputEvent::Enter:
	case QInputEvent::Leave:
		input(dynamic_cast<QMouseEvent*>(e)); break;
	case QInputEvent::KeyPress:
	case QInputEvent::KeyRelease:
	case QInputEvent::FocusIn:
	case QInputEvent::FocusOut:
	case QInputEvent::FocusAboutToChange:
		input(dynamic_cast<QKeyEvent*>(e)); break;
	case QInputEvent::Wheel:
		input(dynamic_cast<QWheelEvent*>(e)); break;
	default:
		qDebug() << "no behavior implemented for event type " << e->type();
	}

}
void TrackStateAbs::setStatus(const std::string& str)

{
    emit m_machineP->setStatus(str.c_str());
}

