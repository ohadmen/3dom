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
	std::array<float, 4> m_color;
	bool m_active;



public:
	Line3d() :m_linWidth(15), m_active(true)
	{


		m_color[0] = 0;
		m_color[1] = 0;
		m_color[2] = 0;
		m_color[3] = 0;
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

	void setLineWidth(float v)
	{
		m_linWidth = v;
	}
	void setColor(float r, float g, float b, float a)
	{
		m_color = { r,g,b,a };
	}
	void draw(const QMatrix4x4& transformMatrix, const QMatrix4x4& viewMatrix)
	{
		if (!m_active)
			return;

		QMatrix4x4 m = viewMatrix*transformMatrix;
		QVector3D p0 = m*m_p0;
		QVector3D p1 = m*m_p1;
		
		
		glColor3f(m_color[0], m_color[1], m_color[2]);
		glLineWidth(m_linWidth);
		glBegin(GL_LINE_STRIP);
		glVertex3f(p0.x(), p0.y(), p0.z());
		glVertex3f(p1.x(), p1.y(), p1.z());
		glEnd();

	
	}





};
#endif