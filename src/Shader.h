#pragma once
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
class Shader: public QOpenGLFunctions
{
	QOpenGLShaderProgram m_shader;
public:
	bool init()
	{
	
		// Enable depth buffer
		glEnable(GL_DEPTH_TEST);

		// Enable back face culling
		glEnable(GL_CULL_FACE);
		// Compile vertex shader
		if (!m_shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/vshader.glsl"))
			return false;

		// Compile fragment shader
		if (!m_shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/fshader.glsl"))
			return false;

		// Link shader pipeline
		if (!m_shader.link())
			return false;

		// Bind shader pipeline for use
		if (!m_shader.bind())
			return false;
		return true;
	}
	void setMVP(const QMatrix4x4& mvp)
	{
		m_shader.setUniformValue("mvp_matrix", mvp);
	}

	QOpenGLShaderProgram& get() { return m_shader; }
};
