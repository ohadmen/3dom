#ifndef BACKDROP_H
#define BACKDROP_H

#include <QtOpenGL/QGLFunctions>
#include <QtOpenGL/QGLShaderProgram>
#include <QtOpenGL/QGLBuffer>

class Backdrop : protected QGLFunctions
{
public:
	Backdrop()
	{


		
	}
	void glinit()
	{
		initializeGLFunctions();
		m_shader.addShaderFromSourceFile(QGLShader::Vertex, ":/gl/quad.vert");
		m_shader.addShaderFromSourceFile(QGLShader::Fragment, ":/gl/quad.frag");
		m_shader.link();

		float vbuf[] = {
			-1.0f, -1.0f, 0.00f, 0.10f, 0.15f,
			-1.0f,  1.0f, 0.03f, 0.21f, 0.26f,
			1.0f, -1.0f, 0.00f, 0.12f, 0.18f,
			1.0f,  1.0f, 0.06f, 0.26f, 0.30f };

		m_vertices.create();
		m_vertices.bind();
		m_vertices.allocate(vbuf, sizeof(vbuf));
		m_vertices.release();
	}
	void draw()
	{
		m_shader.bind();
		m_vertices.bind();

		const GLuint vp = m_shader.attributeLocation("vertex_position");
		const GLuint vc = m_shader.attributeLocation("vertex_color");

		glEnableVertexAttribArray(vp);
		glEnableVertexAttribArray(vc);

		glVertexAttribPointer(vp, 2, GL_FLOAT, false,
			5 * sizeof(GLfloat), 0);
		glVertexAttribPointer(vc, 3, GL_FLOAT, false,
			5 * sizeof(GLfloat),
			(GLvoid*)(2 * sizeof(GLfloat)));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

		m_vertices.release();
		m_shader.release();
	}

    
private:
    QGLShaderProgram m_shader;
    QGLBuffer m_vertices;
};

#endif // BACKDROP_H
