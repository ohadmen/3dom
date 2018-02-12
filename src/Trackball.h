#pragma once
#include<set>
#include <QMatrix4x4>
#include "TrackMode.h"
#include "Qmvp.h"
class Trackball
{
	std::map<int, TrackMode *> m_modes;
	
	int m_currentButton;

	//TODO: needs to move inside modes!!!
	Qmvp m_track;

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
		////I use strict matching.
		//assert(m_modes.count(0));
		//if (!m_modes.count(current_button & MODIFIER_MASK)) {
		//	current_mode = NULL;
		//}
		//else {
		//	current_mode = m_modes[current_button & MODIFIER_MASK];
		//	if (current_mode != NULL)
		//		current_mode->SetAction();
		//}
		//m_lastPoint = QVector3D(0, 0, -1);
		//m_lastTrack = m_track;

	}
	void privButtonUp(const Button& button)
	{
		//bool old_sticky = false, new_sticky = false;
		//assert(m_modes.count(0));
		//
		//Button b = Button(current_button & MODIFIER_MASK);
		//if ((m_modes.count(b)) && (m_modes[b] != NULL)) old_sticky = m_modes[b]->isSticky();
		//
		//current_button &= (~button);
		//b = Button(current_button & MODIFIER_MASK);
		//if ((m_modes.count(b)) && (m_modes[b] != NULL)) new_sticky = m_modes[b]->isSticky();
		//
		//if (!old_sticky && !new_sticky) privSetCurrentAction();

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
		//m_modes[WHEEL] =
		//m_modes[BUTTON_LEFT | KEY_SHIFT] = new ScaleMode();
		//m_modes[BUTTON_LEFT | KEY_ALT] = new ZMode();

	}
public:




	
};