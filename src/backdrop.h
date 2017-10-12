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
		initializeGLFunctions();

		shader.addShaderFromSourceFile(QGLShader::Vertex, ":/gl/quad.vert");
		shader.addShaderFromSourceFile(QGLShader::Fragment, ":/gl/quad.frag");
		shader.link();

		float vbuf[] = {
			-1.0f, -1.0f, 0.00f, 0.10f, 0.15f,
			-1.0f,  1.0f, 0.03f, 0.21f, 0.26f,
			 1.0f, -1.0f, 0.00f, 0.12f, 0.18f,
			 1.0f,  1.0f, 0.06f, 0.26f, 0.30f };

		vertices.create();
		vertices.bind();
		vertices.allocate(vbuf, sizeof(vbuf));
		vertices.release();
	}

	void draw()
	{
		shader.bind();
		vertices.bind();

		const GLuint vp = shader.attributeLocation("vertex_position");
		const GLuint vc = shader.attributeLocation("vertex_color");

		glEnableVertexAttribArray(vp);
		glEnableVertexAttribArray(vc);

		glVertexAttribPointer(vp, 2, GL_FLOAT, false,
			5 * sizeof(GLfloat), 0);
		glVertexAttribPointer(vc, 3, GL_FLOAT, false,
			5 * sizeof(GLfloat),
			(GLvoid*)(2 * sizeof(GLfloat)));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

		vertices.release();
		shader.release();
	}

    
private:
    QGLShaderProgram shader;
    QGLBuffer vertices;
};

#endif // BACKDROP_H
