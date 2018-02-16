#ifndef TRACKSTATE_H
#define TRACKSTATE_H
#include <Qt>
#include <QVector2D>
#include "Qmvp.h"
#include "TrackUtils.h"


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
	};

	class SharedRes
	{
		Qmvp m_track;
		Qmvp m_trackPrev;
		State m_currentState;
		TrackUtils m_tu;
	public:
		SharedRes():m_currentState(State::IDLE){}
		void setTrack(const Qmvp& t)
		{
			m_trackPrev = m_track;
			m_track = t;
		}
		Qmvp getTrack()const { return m_track; }
		Qmvp getTrackPrev()const { return m_trackPrev; }
		void setState(State v) { m_currentState = v; }
		State getState() const { return m_currentState; }
		
		void init()
		{
			m_tu.init();
		}
		TrackUtils& tu() { return m_tu; }
		void resetView(int w, int h, const QVector3D& initpos)
		{
			m_track.resetView(w, h);
			if(initpos!=QVector3D())
				m_track.applyT(initpos);
			m_trackPrev = m_track;
		}
	};
	 

protected:
	SharedRes* m_sr;
	
	const std::map<TrackState::State, TrackState*>& m_statesList;
	
	TrackState* protCurrentState()const  {
		//should be unique!
		//https://stackoverflow.com/questions/1474894/why-isnt-the-operator-const-for-stl-maps
		std::map<TrackState::State, TrackState*>& tt = const_cast<std::map<TrackState::State, TrackState*>&>(m_statesList);
		return tt[m_sr->getState()];
	}
public:
	TrackState(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList):m_sr(sharedRes), m_statesList(stateList){}

	virtual ~TrackState() {
	}

	//virtual void apply(Qt::MouseButtons mb, Qt::KeyboardModifiers kbm,const QPointF& xy,float wheelNotch) { return; }
	virtual void apply(Qt::MouseButtons ,Qt::KeyboardModifiers, const QPointF& , float ) { return; }

	virtual void reset() {}

	virtual const char* name() {return "TrackState";	};

	virtual void draw() { m_sr->tu().drawSphereIcon(m_sr->getTrack(), true); }

};
//----------------------TrackIState_idle----------------------------
class TrackIState_idle :public TrackState {
public:
	TrackIState_idle(SharedRes* sharedRes,const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
	const char *Name() {return "TrackIState_idle";	};

	void draw()
	{
		
		m_sr->tu().drawSphereIcon(m_sr->getTrack(),false);
	}

	void apply(Qt::MouseButtons mb, Qt::KeyboardModifiers kbm,const QPointF& xy, float wheelNotch)
	{ 
		if (mb == Qt::MouseButton::LeftButton && kbm == Qt::KeyboardModifier::NoModifier)
		{
			m_sr->setState(State::ROTATE);
			
		}
		else if (mb == Qt::MouseButton::NoButton && kbm == Qt::KeyboardModifier::NoModifier && wheelNotch != 0)
		{
			m_sr->setState(State::ZOOM);
		}
		protCurrentState()->apply(mb, kbm, xy, wheelNotch);

	}

};

//----------------------TrackIState_rotate----------------------------
class TrackIState_rotate :public TrackState {
public:
	TrackIState_rotate(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
	const char *Name() { return "TrackIState_shpere"; };

	void draw()
	{
		m_sr->tu().drawSphereIcon(m_sr->getTrack(), false);
	}
};



//----------------------TrackIState_zoom----------------------------
class TrackIState_zoom :public TrackState {
public:
	TrackIState_zoom(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
	const char *Name() { return "TrackIState_none"; };
	void apply(Qt::MouseButtons mb, Qt::KeyboardModifiers kbm, const QPointF& xy, float wheelNotch)
	{
		static const float scaleinc = 1.1f;
		static const float scaledec = 1 / scaleinc;
		Qmvp t = m_sr->getTrack();
		t.applyS(wheelNotch > 0 ? scaleinc : scaledec);
		m_sr->setTrack(t);
		m_sr->setState(State::IDLE);
	}

};

//----------------------TrackIState_none----------------------------
class TrackIState_none :public TrackState {
public:
	TrackIState_none(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
	const char *Name() { return "TrackIState_none"; };


};














#endif
