#include "track_state_machine.h"
#include "track_state_idle.h"
#include "libcurie/drawables/drawables_buffer.h"
#include "libcurie/drawables/drawable_basic_shapes.h"
#include "libcurie/common/vp_mat.h"
#include <sstream>


TrackStateMachine::TrackStateMachine() :m_state(new TrackStateIdle(this))
{

}
void TrackStateMachine::setState(TrackStateAbs* newState) {
    delete m_state;
    m_state = newState;
    m_state->m_machineP = this;
    qDebug() << "current state:" << m_state->name();
}

void TrackStateMachine::input(QInputEvent* e)
{
    m_state->input(e); 
}
