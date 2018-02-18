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

	//press/release callback tells us the current state *after* the event
	//to know which button was pressed/released, we need to save the previous state.
	Qt::MouseButtons m_prevMouseState;



	void privInitStates()
	{

		m_states[TrackState::IDLE] = new TrackIState_idle(&m_sr, m_states);
		m_states[TrackState::ROTATE] = new TrackIState_rotate(&m_sr, m_states);
		m_states[TrackState::ZOOM] = new TrackIState_zoom(&m_sr, m_states);
	}
	void privClearStates()
	{
		for (auto s : m_states)
			delete s.second;
	}

public:
	~Trackball() { privClearStates(); }
	Trackball():m_prevMouseState(Qt::MouseButton::NoButton){ privInitStates(); }

	void init()
	{
		m_sr.tu.init();
	}

	void keyPressEvent(QKeyEvent *e)
	{

		//m_states[m_sr.currentState]->applyKeyEvent(e);
	}
	void keyReleaseEvent(QKeyEvent *e)
	{

		//m_states[m_sr.currentState]->applyKeyEvent(e);
	}
	void mouseReleaseEvent(QMouseEvent *e)
	{
		int buttonChange = int(m_prevMouseState) ^ int(e->buttons());
		m_prevMouseState = e->buttons();
		m_states[m_sr.currentState]->apply(-buttonChange, e->modifiers(), e->localPos(), 0);
	}
	void mousePressEvent(QMouseEvent *e)
	{
		int buttonChange = int(m_prevMouseState) ^ int(e->buttons());
		m_prevMouseState = e->buttons();
		m_states[m_sr.currentState]->apply(buttonChange, e->modifiers(), e->localPos(), 0);
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
	void resetView(int w,int h)	    { m_sr.track.resetView(w,h);			}
	void applyT(const QVector3D& t) { m_sr.track.applyT(t); }
	void applyS(float s)            { m_sr.track.applyS(s); }
	void setWinSize(int w, int h) { m_sr.track.setWinSize(w,h); }
};