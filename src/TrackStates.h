#ifndef TRACKSTATE_H
#define TRACKSTATE_H
#pragma warning(disable: 4100)
#include <Qt>
#include <QVector2D>
#include "Qmvp.h"
#include "TrackUtils.h"
#include "mesh.h"
#include "GLpainter.h"

/*
track state is the general state in the state machine of the current control state.
this class hold the shared resource (sr) with the parent, and can alter it (current and
last transformation, and current state. by changing the current state the class can switch
between states. As input, the apply function gets the current mouse buttons, mouse wheel,
and keyboard inputs, apply the transformation, and alters the state if needed
*/
class TrackState {
public:
    
    enum State
    {
        IDLE,
        ROTATE,
        PAN,
        ZOOM,
        FOV,
        RETARGET,
        ZNEAR,
        MEASURE_DISTANCE
    };

    struct SharedRes
    {
        Qmvp    track;
        State   currentState;
        TrackUtils tu;
    
        SharedRes():currentState(State::IDLE){}
        
    };
     

protected:
    SharedRes* m_sr;
    
    const std::map<TrackState::State, TrackState*>& m_statesList;
    
    TrackState* protCurrentState()const  {
        //should be unique!
        //https://stackoverflow.com/questions/1474894/why-isnt-the-operator-const-for-stl-maps
        std::map<TrackState::State, TrackState*>& tt = const_cast<std::map<TrackState::State, TrackState*>&>(m_statesList);
        return tt[m_sr->currentState];
    }
public:
    TrackState(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList):m_sr(sharedRes), m_statesList(stateList){}

    virtual ~TrackState() {
    }

    //virtual void apply(int mb, Qt::KeyboardModifiers kbm,const QPointF& xy,float wheelNotch) { return; }
    virtual void apply(QInputEvent::Type , QMouseEvent* , float ) { return; }
    virtual void apply(QInputEvent::Type t, QKeyEvent*,  const QPointF&) { return; }
    virtual void apply() { return; }

    virtual void reset() {}

    virtual const char* name() {return "TrackState";    };

    virtual void draw() { m_sr->tu.drawSphereIcon(m_sr->track, true); }

};
static std::pair<Qt::MouseButton, bool> sprivMouseButtonState(int mb)
{
    std::pair<Qt::MouseButton, bool> s;
    s.first = Qt::MouseButton(std::abs(mb));
    s.second = mb > 0;
    return s;
}
//----------------------TrackIState_idle----------------------------
class TrackIState_idle :public TrackState {
public:
    TrackIState_idle(SharedRes* sharedRes,const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() {return "TrackIState_idle";    };

    void draw()
    {
        
        m_sr->tu.drawSphereIcon(m_sr->track,false);
    }

    void apply(QInputEvent::Type t,QMouseEvent* e,float wheelNotch)
    { 
      
        
        if (0) {}//tidy :)
        else if (e->button() == Qt::MouseButton::LeftButton && t == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
        {
            m_sr->currentState=(State::ROTATE);

        }
        else if (e->button() == Qt::MouseButton::MidButton && t == QInputEvent::MouseButtonPress && e->modifiers() == Qt::KeyboardModifier::NoModifier)
        {
            m_sr->currentState = (State::PAN);

        }
        else if (e->button() == Qt::MouseButton::NoButton&& e->modifiers() == Qt::KeyboardModifier::NoModifier  && wheelNotch != 0)
        {
            m_sr->currentState=(State::ZOOM);
        }
        else if (e->button() == Qt::MouseButton::NoButton&& e->modifiers() == Qt::KeyboardModifier::ShiftModifier  && wheelNotch != 0)
        {
            m_sr->currentState = (State::ZNEAR);
        }
        else if (e->button() == Qt::MouseButton::NoButton&& e->modifiers() == Qt::KeyboardModifier::ControlModifier  && wheelNotch != 0)
        {
            m_sr->currentState = (State::FOV);
        }
        else if (t == QInputEvent::MouseButtonDblClick && e->modifiers() == Qt::KeyboardModifier::NoModifier )
        {
            m_sr->currentState = (State::RETARGET);
        }
     
        else
            return;//no state change, do not run apply on next state (e.g. next state is idle)
        protCurrentState()->apply(t,e,wheelNotch);

    }

    void apply(QInputEvent::Type t,QKeyEvent* e, const QPointF& xy)
    {
        if (0) {}
        else if (e->key() == Qt::Key_D && t== QInputEvent::KeyPress)
        {
            m_sr->currentState = (State::MEASURE_DISTANCE);
        }
        else
            return;//no state change, do not run apply on next state (e.g. next state is idle)
        protCurrentState()->apply(t,e,xy);
    }
};

//----------------------TrackIState_rotate----------------------------
class TrackIState_rotate :public TrackState {
    Qmvp m_pressTrack;
    QVector3D m_hitOld;
public:
    TrackIState_rotate(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() { return "TrackIState_shpere"; };
    void apply(QInputEvent::Type t,QMouseEvent* e,float wheelNotch)
    {
        
        
        if (e->button() == Qt::MouseButton::LeftButton && t == QInputEvent::MouseButtonPress)//press
        {
            
            m_pressTrack = m_sr->track;
            m_hitOld = m_sr->tu.hitSphere(m_pressTrack, QVector2D(e->localPos()));
            
        }
        else if (e->button() == Qt::MouseButton::LeftButton && t == QInputEvent::MouseButtonRelease)//release
        {

            m_sr->currentState=(State::IDLE);
            protCurrentState()->apply(t,e, 0);
        }
        else if (e->button() == Qt::MouseButton::NoButton)
        {
            static const float rad2deg = 90.0/ std::acos(0.0);

 
            
            QVector3D hitNew = m_sr->tu.hitSphere(m_pressTrack, QVector2D(e->localPos()));
            


            QVector3D axis = QVector3D::crossProduct(hitNew, m_hitOld);
            float phi = std::acos(QVector3D::dotProduct(hitNew, m_hitOld))*rad2deg;
 
            //phi = std::max(phi, (hitNew - m_hitOld).length() / Params::trackBallRadius());
            

            m_sr->track = m_pressTrack;
            m_sr->track.applyR(axis, -phi);
        }
            

        
    }
    
    void draw()
    {
        m_sr->tu.drawSphereIcon(m_sr->track, true);
    }
};

//----------------------TrackIState_pan----------------------------
class TrackIState_pan :public TrackState {
    Qmvp m_pressTrack;
    QVector3D m_hitOld;
public:
    TrackIState_pan(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() { return "TrackIState_pan"; };
    void apply(QInputEvent::Type t,QMouseEvent* e,float wheelNotch)
    {

        
        if (e->button() == Qt::MouseButton::MidButton && t == QInputEvent::MouseButtonPress)//press
        {
            m_pressTrack = m_sr->track;
            m_hitOld = m_sr->tu.hitViewPlane(m_pressTrack, QVector2D(e->localPos()),false);

        }
        else if (e->button() == Qt::MouseButton::MidButton && t == QInputEvent::MouseButtonRelease)//release
        {
            m_sr->currentState = (State::IDLE);
            protCurrentState()->apply(t,e, 0);
        }
        else if (e->button() == Qt::MouseButton::NoButton)
        {

            QVector3D hitNew = m_sr->tu.hitViewPlane(m_pressTrack, QVector2D(e->localPos()),false);

            m_sr->track = m_pressTrack;
            m_sr->track.applyT((m_hitOld- hitNew),false);
        }



    }

    void draw()
    {
        m_sr->tu.drawSphereIcon(m_sr->track, true);
    }
};

//----------------------TrackIState_zoom----------------------------
class TrackIState_zoom :public TrackState {
public:
    TrackIState_zoom(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() { return "TrackIState_zoom"; };
    void apply(QInputEvent::Type t, QMouseEvent* e, float wheelNotch)
    {
       
        if (e->button() == Qt::MouseButton::NoButton&& e->modifiers()==Qt::KeyboardModifier::NoModifier)
        {
            //Qmvp mvp = m_sr->tu.getShpereMVP(m_sr->track);

            static const float scaleinc = 1.1f;
            static const float scaledec = 1.0f / scaleinc;
            float s = wheelNotch > 0 ? scaleinc : scaledec;
            
       
            m_sr->track.applyS(s);

            m_sr->currentState = (State::IDLE);
        }
		
    }

};

//----------------------TrackIState_retarget----------------------------
class TrackIState_retarget :public TrackState {
    int* m_currentMeshTokenP;
public:
    TrackIState_retarget(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList,int* currentMeshTokenP) :TrackState(sharedRes, stateList), m_currentMeshTokenP(currentMeshTokenP){}
    const char *Name() { return "TrackIState_retarget"; };
    void apply(QInputEvent::Type t, QMouseEvent* e, float wheelNotch)
    {
        const Mesh* p = MeshArray::i().getMesh(*m_currentMeshTokenP);
        if (p != nullptr)
        {
            QLine3D ll = m_sr->track.cameraRay(QVector2D(e->localPos()));
			QVector3D pt;
			if (p->closest2ray(ll, &pt))
			{
				pt[0] *= -1; //??????????
				m_sr->track.setT(pt, false);
			}
            //GLpainter::i().addDrawLine(ll);
            GLpainter::i().setStatus(pt);

           
        }
     
        

            m_sr->currentState = (State::IDLE);
        
    }

};

//----------------------TrackIState_znear----------------------------
class TrackIState_znear :public TrackState {

public:
    TrackIState_znear(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() { return "TrackIState_znear"; };
    void apply(QInputEvent::Type t, QMouseEvent* e, float wheelNotch)
    {
        const float scaleinc = 1.01f;
        const float scaledec = 1.0f / scaleinc;
        float s = wheelNotch > 0 ? scaleinc : scaledec;

        float ov = Params::camZnear()*s;
        
        Params::camZnear(ov);
        GLpainter::i().setStatus("Z-near " + QString::number(ov));
        
        m_sr->currentState = (State::IDLE);
        m_sr->track.recalcProjMat();
    }

};

//----------------------TrackIState_fov----------------------------
class TrackIState_fov :public TrackState {

public:
    TrackIState_fov(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() { return "TrackIState_fov"; };
    void apply(QInputEvent::Type t, QMouseEvent* e, float wheelNotch)
    {
        const float scaleinc = 1.05f;
        const float scaledec = 1.0f / scaleinc;
        float s = wheelNotch > 0 ? scaleinc : scaledec;

        float ov = Params::camFOV()*s;
        ov = std::max(1.0f, std::min(90.0f, ov));
        Params::camFOV(ov);
        GLpainter::i().setStatus("FOV " + QString::number(ov));
        
        m_sr->currentState = (State::IDLE);
        m_sr->track.recalcProjMat();
    }

};

//----------------------TrackIState_measureDistance----------------------------
class TrackIState_measureDistance :public TrackState {
    bool m_measuring;
    QVector3D m_origin;
    int* m_currentMeshTokenP;
public:
    TrackIState_measureDistance(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList, int* currentMeshTokenP) :TrackState(sharedRes, stateList), m_currentMeshTokenP(currentMeshTokenP), m_measuring(false),m_origin(0,0,0){}
    const char *Name() { return "TrackIState_measureDistance"; };
    void apply(QInputEvent::Type t,QKeyEvent* e, const QPointF& xy)
    
    {
        m_sr->currentState = (State::IDLE);
        if (e->key() != Qt::Key_D || t!= QInputEvent::KeyPress)
            return;

        const Mesh* p = MeshArray::i().getMesh(*m_currentMeshTokenP);
        if (p == nullptr)
            return;
        
        QLine3D ll = m_sr->track.cameraRay(QVector2D(xy));
        QVector3D pt;
        if (!p->closest2ray(ll, &pt))
            return;

        if (m_measuring)
        {
            m_measuring = false;
            float dist = (pt - m_origin).length();
            GLpainter::i().setStatus("Distance="+ QString::number(dist));
            m_sr->currentState = (State::IDLE);
            
        }
        else
        {
            m_measuring = true;
            m_origin = pt;
            m_sr->currentState = (State::MEASURE_DISTANCE);
        }
        
        m_sr->track.recalcProjMat();
    }

};




//----------------------TrackIState_none----------------------------
class TrackIState_none :public TrackState {
public:
    TrackIState_none(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
    const char *Name() { return "TrackIState_none"; };


};














#endif
