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
    QPointF m_mousepos;
    QOpenGLWidget* m_parent;

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
        m_states[TrackState::MEASURE_DISTANCE] = new TrackIState_measureDistance(&m_sr, m_states, m_currentMeshTokenP);
        m_states[TrackState::PRINT_SCREEN] = new TrackIState_printScreen(&m_sr, m_states,m_parent);
        
    }
    void privClearStates()
    {
        for (auto s : m_states)
            delete s.second;
    }

public:
    ~Trackball(){ privClearStates(); }
    Trackball(QOpenGLWidget* parent):m_parent(parent) , m_currentMeshTokenP(nullptr), m_mousepos(0,0){  }

    void init(int* currentMeshTokenP)
    {
        m_currentMeshTokenP = currentMeshTokenP;
        m_sr.tu.init();
        privInitStates();
    }

    void keyPressEvent(QKeyEvent *e)
    {
        m_states[m_sr.currentState]->apply(QInputEvent::KeyPress,e, m_mousepos);
    
    }
    void keyReleaseEvent(QKeyEvent *e)
    {
        
        m_states[m_sr.currentState]->apply(QInputEvent::KeyRelease ,e, m_mousepos);


    }
    void mouseReleaseEvent(QMouseEvent *e)
    {
        
        m_states[m_sr.currentState]->apply(QInputEvent::MouseButtonRelease,e, 0);
    }
    void mousePressEvent(QMouseEvent *e)
    {
        m_states[m_sr.currentState]->apply(QInputEvent::MouseButtonPress, e, 0);
    }
    void mouseDoubleClickEvent(QMouseEvent *e)
    {
        
        //map double click to ExtraButton10 
        m_states[m_sr.currentState]->apply(QInputEvent::MouseButtonDblClick, e, 0);
    }
    void mouseMoveEvent(QMouseEvent *e)
    {
        m_mousepos = e->localPos();
        m_states[m_sr.currentState]->apply(QInputEvent::MouseMove, e, 0);
    }
    void wheelEvent(QWheelEvent *e)
    {
        QMouseEvent ee(QMouseEvent::Wheel, e->posF(), Qt::NoButton, e->buttons(), e->modifiers());
        m_states[m_sr.currentState]->apply(QInputEvent::Wheel,&ee, e->delta());
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