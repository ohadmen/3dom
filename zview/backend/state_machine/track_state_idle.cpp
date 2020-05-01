#include "track_state_idle.h"
#include "track_state_zoom.h"
#include "track_state_rotate.h"
#include "track_state_translate.h"
#include "track_state_retarget.h"
#include "track_state_measure_distance.h"
#include "state_machine.h"
#include <cmath>
#include <sstream>
#include "zview/common/params.h"
constexpr float inf = std::numeric_limits<float>::infinity();

TrackStateIdle::TrackStateIdle(TrackStateMachine *machine) : TrackStateAbs(machine), m_mousepos(-1, -1) {}

void TrackStateIdle::input(QKeyEvent *e)
{

    bool noModifier = e->modifiers() == Qt::KeyboardModifier::NoModifier;
    bool typeKeypress = e->type() == QInputEvent::KeyRelease;
    if (typeKeypress && noModifier && e->key() >= Qt::Key::Key_0 && e->key() <= Qt::Key::Key_9)
    {
        int k(e->key() - Qt::Key::Key_0);
        m_machineP->setTexture(k);
        m_machineP->canvasUpdate();
    }
    else if (typeKeypress && noModifier && e->key() == Qt::Key::Key_D)
    {

        TrackStateMeasureDistance *tsmd = m_machineP->getState<TrackStateMeasureDistance>();

        if (tsmd->setMesuringStartPoint(m_mousepos))
            m_machineP->setCurrentState<TrackStateMeasureDistance>();
    }
}

void TrackStateIdle::input(QWheelEvent *e)
{
    if (e->type() == QInputEvent::Wheel && e->modifiers() == Qt::KeyboardModifier::NoModifier)
        TrackStateZoom(m_machineP).input(e);
    else if (e->type() == QInputEvent::Wheel && e->modifiers() == Qt::KeyboardModifier::AltModifier)
    {
        float newps = Params::pointSize() + 0.2 * (e->delta() < 0 ? 1 : -1);
        newps = std::max(std::min(10.0f, newps), 0.1f);
        Params::pointSize(newps);
        m_machineP->setStatus("pointSize set to "+std::to_string(newps));
    }
    else if (e->type() == QInputEvent::Wheel && e->modifiers() == Qt::KeyboardModifier::ShiftModifier )
    {
        float newval = Params::camZfar() * (e->delta() > 0 ? 1 / 0.9 : 0.9);
        Params::camZfar(newval);
        m_machineP->updatePmat();
        m_machineP->setStatus("Z far set to "+std::to_string(newval));
    }
    else if (e->type() == QInputEvent::Wheel && e->modifiers() == Qt::KeyboardModifier::ControlModifier )
    {
        float newval = Params::camZnear() * (e->delta() > 0 ? 1 / 0.9 : 0.9);
        Params::camZnear(newval);
        m_machineP->updatePmat();
        m_machineP->setStatus("Z near set to "+std::to_string(newval));
    }

    m_machineP->canvasUpdate();
}
void TrackStateIdle::input(QMouseEvent *e)
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
