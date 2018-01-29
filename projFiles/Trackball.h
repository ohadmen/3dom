#pragma once
#include<set>
#include <QMatrix4x4>
#include <QtOpenGL/QGLFunctions>
#include "View.h"
#include "TrackMode.h"
#include "Similarity.h"
class Trackball
{

	enum Button {
		BUTTON_NONE = 0x0000, ///< No button or key pressed.
		BUTTON_LEFT = 0x0001, ///< Left mouse button pressed.
		BUTTON_MIDDLE = 0x0002, ///< Middle mouse button pressed.
		BUTTON_RIGHT = 0x0004, ///< Right mouse button pressed.
		WHEEL = 0x0008, ///< Mouse wheel activated.
		KEY_SHIFT = 0x0010, ///< Shift key pressed.
		KEY_CTRL = 0x0020, ///< Ctrl key pressed.
		KEY_ALT = 0x0040, ///< Alt key pressed.
		HANDLE = 0x0080, ///< Application-defined state activated.
		MODIFIER_MASK = 0x00FF, ///< (mask to get modifiers only)
		KEY_UP = 0x0100, ///< Up directional key
		KEY_DOWN = 0x0200, ///< Down directional key
		KEY_LEFT = 0x0400, ///< Left directional key
		KEY_RIGHT = 0x0800, ///< Right directional key
		KEY_PGUP = 0x1000, ///< PageUp directional key
		KEY_PGDOWN = 0x2000, ///< PageDown directional key
	};
	static Trackball::Button privq2b(const Qt::MouseButton& qtbt, const Qt::KeyboardModifiers& modifiers)
	{
		int vcgbt = Trackball::BUTTON_NONE;
		if (qtbt & Qt::LeftButton) vcgbt |= Trackball::BUTTON_LEFT;
		if (qtbt & Qt::RightButton) vcgbt |= Trackball::BUTTON_RIGHT;
		if (qtbt & Qt::MidButton) vcgbt |= Trackball::BUTTON_MIDDLE;
		if (modifiers & Qt::ShiftModifier)	vcgbt |= Trackball::KEY_SHIFT;
		if (modifiers & Qt::ControlModifier) vcgbt |= Trackball::KEY_CTRL;
		if (modifiers & Qt::AltModifier) vcgbt |= Trackball::KEY_ALT;
		return Trackball::Button(vcgbt);
	}
	void privSetCurrentAction()
	{
		//I use strict matching.
		assert(m_modes.count(0));
		if (!m_modes.count(current_button & MODIFIER_MASK)) {
			current_mode = NULL;
		}
		else {
			current_mode = m_modes[current_button & MODIFIER_MASK];
			if (current_mode != NULL)
				current_mode->SetAction();
		}
		m_lastPoint = QVector3D(0, 0, -1);
		m_lastTrack = m_track;

	}
	void privButtonUp(const Button& button)
	{
		bool old_sticky = false, new_sticky = false;
		assert(m_modes.count(0));

		Button b = Button(current_button & MODIFIER_MASK);
		if ((m_modes.count(b)) && (m_modes[b] != NULL)) old_sticky = m_modes[b]->isSticky();

		current_button &= (~button);
		b = Button(current_button & MODIFIER_MASK);
		if ((m_modes.count(b)) && (m_modes[b] != NULL)) new_sticky = m_modes[b]->isSticky();

		if (!old_sticky && !new_sticky) privSetCurrentAction();

	}
	void privClearModes()
	{
		// Note: people ofter maps different keys to the same modes.
		// so we should avoid double deletion of these double referenced modes.
		std::set<TrackMode *> goodModes;
		std::map<int, TrackMode *>::iterator it;
		for (it = m_modes.begin(); it != m_modes.end(); it++)
			if ((*it).second) goodModes.insert((*it).second);

		std::set<TrackMode *>::iterator its;
		for (its = goodModes.begin(); its != goodModes.end(); its++)
			delete *its;

		m_modes.clear();

	}
	void privSetDefaultMapping() {
		m_idle_and_keys_mode = NULL;

		m_inactive_mode = new InactiveMode();
		privClearModes();
		m_modes[0] = NULL;
		m_modes[BUTTON_MIDDLE | KEY_ALT] =
		m_modes[BUTTON_LEFT] = new SphereMode();
		m_modes[BUTTON_LEFT | KEY_CTRL] = new PanMode();
		m_modes[BUTTON_MIDDLE] = new PanMode();
		m_modes[WHEEL] =
		m_modes[BUTTON_LEFT | KEY_SHIFT] = new ScaleMode();
		m_modes[BUTTON_LEFT | KEY_ALT] = new ZMode();

	}
public:


	const QVector3D& getCenter() const { return m_center; }
	float getRadius() const { return m_radius; }

	const Similarity& getTrack()const { return m_track; }
	      Similarity& track()      { return m_track; }

    const Similarity& getTrackPrev()const { return m_lastTrack; }
		  Similarity& getTrackPrev()      { return m_lastTrack; }

		  const View& getCamera()const { return m_camera; }

	void pushHit(const QVector3D& h)	{		m_hits.push_back(h);	}
	const QVector3D& getLastPoint()const { return m_lastPoint; }

	~Trackball()
	{
		privClearModes();
		delete  m_inactive_mode;
	}
	Trackball() :m_radius(1), m_center(0, 0, 0) {
		privSetDefaultMapping();
	}




	void scale(const float s)
	{
		m_track.scale() *= s;
	}

	void translate(const QVector3D& tr)
	{
		QQuaternion irot = m_track.rotation();
		
		m_track.SetTranslate(m_lastTrack.translate() + irot.inverted()*tr / m_track.scale());
	}


	void MouseMove(int x, int y) {
		if (current_mode == NULL) return;
		if (m_lastPoint[2] == -1) { //changed mode in the middle of moving
			m_lastPoint = QVector3D((float)x, (float)y, 0);
			return;
		}
		m_undoTrack = m_track;
		current_mode->Apply(*this, QVector3D(float(x), float(y), 0));
	}
	void callback_mouseDown(int x, int y, const Qt::MouseButton& qtbt, const Qt::KeyboardModifiers& modifiers) 
	{
		Trackball::Button button = privq2b(qtbt, modifiers);
		m_undoTrack = m_track;
		current_button |= button;
		privSetCurrentAction();
		m_lastPoint = QVector3D((float)x, (float)y, 0);
		m_hits.clear();

	}
	void callback_mouseUp(int x, int y, const Qt::MouseButton& qtbt, const Qt::KeyboardModifiers& modifiers) {
		m_undoTrack = m_track;
		Trackball::Button button = privq2b(qtbt,modifiers);
		privButtonUp(button);
		//current_button &= (~button);
		//SetCurrentAction();
	}


private:
	std::map<int, TrackMode *> m_modes;
	View m_camera;
	int current_button;
	//TODO: needs to move inside modes!!!
	TrackMode* current_mode;
	TrackMode* m_idle_and_keys_mode;
	TrackMode* m_inactive_mode;


	QVector3D m_center;
	float m_radius;
	Similarity m_track;
	Similarity m_undoTrack;
	Similarity m_lastTrack;
	std::vector<QVector3D> m_hits;

	QVector3D m_lastPoint;
};