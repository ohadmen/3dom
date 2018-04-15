#ifndef BACKDROP_H
#define BACKDROP_H


#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "Params.h"

class Backdrop : protected QOpenGLFunctions
{
public:
    
    Backdrop() {}
        void init()
	{
        initializeOpenGLFunctions();
        initShader("meshv", "meshf");
		m_meshShader.link();

		float vbuf[] = {
			-1.0f, -1.0f,-Params::camZfar(), 0.00f, 0.10f, 0.15f,
			-1.0f,  1.0f,-Params::camZfar(), 0.03f, 0.21f, 0.26f,
			 1.0f, -1.0f,-Params::camZfar(), 0.00f, 0.12f, 0.18f,
			 1.0f,  1.0f,-Params::camZfar(), 0.06f, 0.26f, 0.30f };

		m_verts.create();
		m_verts.bind();
		m_verts.allocate(vbuf, sizeof(vbuf));
		m_verts.release();
	}
        bool initShader(const QString& vshader, const QString& fshader)
        {
            // Compile vertex shader
            if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/" + vshader + ".glsl"))
                return false;

            // Compile fragment shader
            if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/" + fshader + ".glsl"))
                return false;

            // Link shader pipeline
            if (!m_meshShader.link())
                return false;
            return true;

        }


	void draw()
	{
        m_meshShader.bind();
		m_verts.bind();
        QMatrix4x4 eye; eye.setToIdentity();
        m_meshShader.setUniformValue("mvp_matrix", eye);
		const GLuint vp = m_meshShader.attributeLocation("a_xyz");
		const GLuint vc = m_meshShader.attributeLocation("a_rgb");
        m_meshShader.setUniformValue("u_txt", 0);

		glEnableVertexAttribArray(vp);
		glVertexAttribPointer(vp, 3, GL_FLOAT, false,6 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(vc);
        glVertexAttribPointer(vc, 3, GL_FLOAT, false,6 * sizeof(GLfloat),
			(GLvoid*)(2 * sizeof(GLfloat)));

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

		m_verts.release();
        m_meshShader.release();
	}

    
private:
    QOpenGLShaderProgram m_meshShader;
    QOpenGLBuffer m_verts;
};

#endif // BACKDROP_H
