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
	Trackball() { privInitStates(); }

	void init()
	{
		m_sr.init();
	}

	void keyPressEvent(QKeyEvent *e)
	{

		//m_states[m_sr.getState()]->applyKeyEvent(e);
	}
	void keyReleaseEvent(QKeyEvent *e)
	{

		//m_states[m_sr.getState()]->applyKeyEvent(e);
	}
	void mouseReleaseEvent(QMouseEvent *e)
	{
		m_states[m_sr.getState()]->apply(e->buttons(), e->modifiers(), e->localPos(), 0);
	}
	void mousePressEvent(QMouseEvent *e)
	{
		m_states[m_sr.getState()]->apply(e->buttons(), e->modifiers(), e->localPos(), 0);
	}
	void wheelEvent(QWheelEvent *e)
	{
		m_states[m_sr.getState()]->apply(e->buttons(), e->modifiers(), e->posF(), e->delta());
	}

	void draw()
	{
		m_states[m_sr.getState()]->draw();
	}
	Qmvp getMVP()const
	{
		return m_sr.getTrack();
	}
	void resetView(int w,int h,const QVector3D& initpos)
	{
		m_sr.resetView(w,h,initpos);
		
	}

};