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
		State m_currentState;
		TrackUtils m_tu;
	public:
		SharedRes():m_currentState(State::IDLE){}
		void setTrack(const Qmvp& t){m_track = t;		}
		Qmvp getTrack()const { return m_track; }
		void setState(State v) { m_currentState = v; }
		State getState() const { return m_currentState; }
		
		void init()
		{
			m_tu.init();
		}
		TrackUtils& tu() { return m_tu; }
		void resetView(int w, int h)
		{
			m_track.resetView(w, h);
		}
		void applyT(const QVector3D& t)		{			m_track.applyT(t);		}
		void applyS(float s) { m_track.applyS(s); }
		void applyR(const QVector3D& axis, float angle) { m_track.applyR(axis, angle); }
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

	//virtual void apply(int mb, Qt::KeyboardModifiers kbm,const QPointF& xy,float wheelNotch) { return; }
	virtual void apply(int ,Qt::KeyboardModifiers, const QPointF& , float ) { return; }

	virtual void reset() {}

	virtual const char* name() {return "TrackState";	};

	virtual void draw() { m_sr->tu().drawSphereIcon(m_sr->getTrack(), true); }

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
	const char *Name() {return "TrackIState_idle";	};

	void draw()
	{
		
		m_sr->tu().drawSphereIcon(m_sr->getTrack(),false);
	}

	void apply(int mb_, Qt::KeyboardModifiers kbm,const QPointF& xy, float wheelNotch)
	{ 
		auto mb = sprivMouseButtonState(mb_);

		if (mb.first == Qt::MouseButton::LeftButton && mb.second && kbm == Qt::KeyboardModifier::NoModifier)
		{
			m_sr->setState(State::ROTATE);

		}
		else if (mb.first == Qt::MouseButton::NoButton && kbm == Qt::KeyboardModifier::NoModifier && wheelNotch != 0)
		{
			m_sr->setState(State::ZOOM);
		}
		else
			return;//no state change, do not run apply on next state (e.g. next state is idle)
		protCurrentState()->apply(mb_, kbm, xy, wheelNotch);

	}

};

//----------------------TrackIState_rotate----------------------------
class TrackIState_rotate :public TrackState {
	Qmvp m_pressTrack;
	QVector3D m_pressVec;
public:
	TrackIState_rotate(SharedRes* sharedRes, const std::map<TrackState::State, TrackState*>& stateList) :TrackState(sharedRes, stateList) {}
	const char *Name() { return "TrackIState_shpere"; };
	void apply(int mb_, Qt::KeyboardModifiers kbm, const QPointF& xy, float wheelNotch)
	{
		
		auto mb = sprivMouseButtonState(mb_);
		if (mb.first == Qt::MouseButton::LeftButton && mb.second)//press
		{
			m_pressTrack = m_sr->getTrack();
			m_pressVec = m_pressTrack.getViewVector(QVector2D(xy));
			
		}
		else if (mb.first == Qt::MouseButton::LeftButton && !mb.second)//release
		{
			m_sr->setState(State::IDLE);
			protCurrentState()->apply(0, kbm, xy, 0);
		}
		else if (mb.first == Qt::MouseButton::NoButton)
		{
			static const float rad2deg = 90.0/ std::acos(0.0);
			QVector3D vv = m_pressTrack.getViewVector(QVector2D(xy));
			m_sr->setTrack(m_pressTrack);
			QVector3D axis = QVector3D::crossProduct(vv, m_pressVec);
			float angle = std::acos(QVector3D::dotProduct(vv, m_pressVec))*rad2deg;
			m_sr->applyR(axis,angle);
		}
			

		
	}
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
	void apply(int mb, Qt::KeyboardModifiers kbm, const QPointF& xy, float wheelNotch)
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
