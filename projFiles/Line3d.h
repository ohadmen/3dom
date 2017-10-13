#ifndef LINE3D_H
#define LINE3D_H
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLFunctions>
#include <QVector3D>
#include <vector>
#include <map>
#include <array>
class Line3d
{
	QVector3D m_p0;
	QVector3D m_p1;
	float m_linWidth;
	std::array<float, 3> m_color;
	bool m_active;



public:
	Line3d() :m_linWidth(15), m_active(true)
	{


		m_color[0] = 0;
		m_color[1] = 0;
		m_color[2] = 0;
	}


	void set(const QVector3D& p0,const QVector3D& p1)
	{

		m_p0 = p0;
		m_p1 = p1;

	}

	bool active(int v = -1)
	{
		if (v != -1)
			m_active = v;
		return m_active;
	}

	void draw()
	{
		if (!m_active)
			return;


		// Save matrix state and do the custom rotation
		glPushMatrix();
		glColor3f(m_color[0], m_color[1], m_color[2]);
		glLineWidth(m_linWidth);
		glBegin(GL_LINE_STRIP);
		glVertex3f(m_p0.x(), m_p0.y(), m_p0.z());
		glVertex3f(m_p1.x(), m_p1.y(), m_p1.z());
		glEnd();

		// Restore the matrix state
		glPopMatrix();
	}





};
#endif