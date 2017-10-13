#ifndef CIRCLE_H
#define CIRCLE_H
#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLFunctions>
#include <QVector3D>
#include <vector>
#include <map>
#include <array>
class Circle3d
{
	typedef std::vector<QVector3D> Pgon;
	Pgon m_pgn;
	float m_linWidth;
	std::array<float, 3> m_color;
	bool m_active;
	Pgon m_vertices;


	static Pgon getCirclePoints(int nSegments)
	{
		static const float pi = std::acos(0.0) * 2;
		static std::map<int, Pgon> vectContainer;
		if (vectContainer.find(nSegments) == vectContainer.end())
		{
			float theta = 2 * pi / float(nSegments);
			Pgon res(nSegments);
			for (int ii = 0; ii != nSegments; ii++)
			{
				res[ii] = QVector3D(std::cos(theta*ii), std::sin(theta*ii), 0);


			}
			vectContainer[nSegments] = std::move(res);

		}
		return vectContainer[nSegments];
	}

public:
	Circle3d(int nsegments = 64) :m_pgn(getCirclePoints(nsegments)), m_linWidth(15), m_active(true)
	{
		
		m_vertices.resize(nsegments);
		set(QMatrix4x4());
		m_color[0] = 0;
		m_color[1] = 0;
		m_color[2] = 0;
	}


	void set(const QMatrix4x4& t)
	{
		
		for (int ii = 0; ii != m_pgn.size(); ii++)
		{
			m_vertices[ii] = t*m_pgn[ii];
		}

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
		glBegin(GL_LINE_LOOP);
		for (int ii = 0; ii != m_vertices.size(); ii++)
		{
			glVertex3f(m_vertices[ii].x(), m_vertices[ii].y(), m_vertices[ii].z());
		}

		glEnd();

		// Restore the matrix state
		glPopMatrix();
	}





};
#endif