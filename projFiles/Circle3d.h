#ifndef CIRCLE_H
#define CIRCLE_H
#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLBuffer>

#include <vector>
#include <map>
#include <array>
class Circle3d : protected QGLFunctions
{
	typedef std::vector<QVector3D> Pgon;
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
	Circle3d(int nsegments = 128) :m_pgn(getCirclePoints(nsegments)), m_linWidth(15), m_active(true)
	{
		
		m_pgnT = m_pgn;
		m_color[0] = 0;
		m_color[1] = 1;
		m_color[2] = 0;
		m_color[3] = 1;
	}
	void setLineWidth(float v)
	{
		m_linWidth = v;
	}
	void setColor(float r, float g, float b, float a=1)
	{
		m_color = { r,g,b,a };
	}

	

	void set(const QMatrix4x4& t)
	{
		
		for (int ii = 0; ii != m_pgn.size(); ii++)
		{
			m_pgnT[ii] = t*m_pgn[ii];
		}


	}

	bool active(int v = -1)
	{
		if (v != -1)
			m_active = v;
		return m_active;
	}

	void draw(const QMatrix4x4& transformMatrix, const QMatrix4x4& viewMatrix)
	{
		if (!m_active)
			return;
		
		
		
		
		//m_shader.bind();
		glLineWidth(m_linWidth);
		glColor4f(m_color[0], m_color[1], m_color[2], m_color[3]);
		//m_vertices.bind();
		//glUniformMatrix4fv(
		//	m_shader.uniformLocation("transform_matrix"),
		//	1, GL_FALSE, transformMatrix.data());
		//glUniformMatrix4fv(
		//	m_shader.uniformLocation("view_matrix"),
		//	1, GL_FALSE, viewMatrix.data());



		
			glBegin(GL_LINE_LOOP);


		for (int i=0;i!= m_pgnT.size();++i)
		{
			QVector3D t = viewMatrix*transformMatrix*m_pgnT[i];
			// Specify the point and move the z value up a little 
			glVertex3f(t.x(), t.y(), t.z());
			
		}

		// Done drawing points
		glEnd();
		//m_shader.release();


	}




private:
	
	Pgon m_pgn;
	Pgon m_pgnT;
	float m_linWidth;
	std::array<float, 4> m_color;
	bool m_active;

	//QGLShaderProgram m_shader;
	//QGLBuffer m_vertices;

};
#endif