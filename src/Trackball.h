#pragma once
#include <map>
#include <QMatrix4x4>
#include <QWidget>
#include "TrackStates.h"
#include "Qmvp.h"
#include <vector>

class Trackball
{

    TrackState::SharedRes m_sr;
    std::map<TrackState::State, TrackState*> m_states;
    int* m_currentMeshTokenP;

    //press/release callback tells us the current state *after* the event
    //to know which button was pressed/released, we need to save the previous state.
    



    void privInitStates()
    {
        privClearStates();
        m_states[TrackState::IDLE] = new TrackIState_idle(&m_sr, m_states);
        m_states[TrackState::ROTATE] = new TrackIState_rotate(&m_sr, m_states);
        m_states[TrackState::ZOOM] = new TrackIState_zoom(&m_sr, m_states);
        m_states[TrackState::ZNEAR] = new TrackIState_znear(&m_sr, m_states);
        m_states[TrackState::PAN] = new TrackIState_pan(&m_sr, m_states);
        m_states[TrackState::FOV] = new TrackIState_fov(&m_sr, m_states);
        m_states[TrackState::RETARGET] = new TrackIState_retarget(&m_sr, m_states,m_currentMeshTokenP);
        m_states[TrackState::MEASURE_DISTANCE] = new TrackIState_measureDistance(&m_sr, m_states);
        
    }
    void privClearStates()
    {
        for (auto s : m_states)
            delete s.second;
    }

public:
    ~Trackball() { privClearStates(); }
    Trackball(): m_currentMeshTokenP(nullptr){  }

    void init(int* currentMeshTokenP)
    {
        m_currentMeshTokenP = currentMeshTokenP;
        m_sr.tu.init();
        privInitStates();
    }

    void keyPressEvent(QKeyEvent *e)
    {
        qDebug() << e->key();
        e;
    }
    void keyReleaseEvent(QKeyEvent *e)
    {

        e;
    }
    void mouseReleaseEvent(QMouseEvent *e)
    {
        m_states[m_sr.currentState]->apply(-int(e->button()), e->modifiers(), e->localPos(), 0);
    }
    void mousePressEvent(QMouseEvent *e)
    {
        m_states[m_sr.currentState]->apply(int(e->button()), e->modifiers(), e->localPos(), 0);
    }
    void mouseDoubleClickEvent(QMouseEvent *e)
    {
        //map double click to ExtraButton10 
        m_states[m_sr.currentState]->apply(int(Qt::MouseButton::ExtraButton10), e->modifiers(), e->localPos(), 0);
    }
    void mouseMoveEvent(QMouseEvent *e)
    {
        m_states[m_sr.currentState]->apply(Qt::MouseButton::NoButton, e->modifiers(), e->localPos(), 0);
    }
    void wheelEvent(QWheelEvent *e)
    {
        m_states[m_sr.currentState]->apply(e->buttons(), e->modifiers(), e->posF(), e->delta());
    }

    void draw()
    {
        m_states[m_sr.currentState]->draw();
        
    }
    Qmvp getMVP()const
    {
        return m_sr.track;
    }
    void resetView(int w,int h)        {
        m_sr.track.reset(w,h);   
        m_sr.track.setT(QVector3D(0, 0, -1));//camera is at 0,0,-1 look up
    }
    void applyT(const QVector3D& t,bool view=true) { m_sr.track.applyT(t,view); }
    void applyS(float s,bool view = true)            { m_sr.track.applyS(s,view); }
    void setWinSize(int w, int h) { m_sr.track.setWinSize(w,h); }
};