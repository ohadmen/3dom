#pragma once
#include <QMatrix4x4>
#include <QtOpenGL/QGLFunctions>
#include "View.h"
#include "TrackMode.h"
class Trackball: public QTransform
{
public:


	const QVector3D center() const { return m_center; }
	float radius() const { return m_radius; }

	const QTransform& getTrack()const { return m_track; }
	const View& getCamera()const { return m_camera; }
private:

	View m_camera;
	int current_button;
	TrackMode* current_mode;
	QVector3D m_center;
	float m_radius;
	QTransform m_track;
};