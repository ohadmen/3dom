#ifndef GLMESH_H
#define GLMESH_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "mesh.h"

class GLMesh : protected QOpenGLFunctions
{
public:
	~GLMesh()
	{
		if (m_vertices.isCreated())
			m_vertices.destroy();
		if (m_indices.isCreated())
			m_indices.destroy();
	}
	GLMesh() : m_vertices(QOpenGLBuffer::VertexBuffer), m_indices(QOpenGLBuffer::IndexBuffer)
	{
		// Enable depth buffer
		glEnable(GL_DEPTH_TEST);

		// Enable back face culling
		glEnable(GL_CULL_FACE);
	}
	void initShader()
	{
		//static const char *vertexShaderSource =
		//	"attribute highp vec4 posAttr;\n"
		//	"attribute lowp vec4 colAttr;\n"
		//	"varying lowp vec4 col;\n"
		//	"uniform highp mat4 matrix;\n"
		//	"void main() {\n"
		//	"   col = colAttr;\n"
		//	"   gl_Position = matrix * posAttr;\n"
		//	"}\n";

		//static const char *fragmentShaderSource =
		//	"varying lowp vec4 col;\n"
		//	"void main() {\n"
		//	"   gl_FragColor = col;\n"
		//	"}\n";
		//
		initializeOpenGLFunctions();
		m_meshShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/gl/mesh.vert");
		m_meshShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/gl/mesh.frag");
		m_meshShader.link();
	}
	void set(const Mesh&  mesh)
		
	{
		initializeOpenGLFunctions();

		m_vertices.create();
		m_indices.create();

		m_vertices.setUsagePattern(QOpenGLBuffer::StaticDraw);
		m_indices.setUsagePattern(QOpenGLBuffer::StaticDraw);

		m_vertices.bind();
		m_vertices.allocate(mesh.vertData(),	int(mesh.vertSize() * sizeof(float)*3));
		m_vertices.release();

		m_indices.bind();
		m_indices.allocate(mesh.indData(),int(mesh.indSize() * sizeof(uint32_t)));
		m_indices.release();
		
	}
    void draw(const QMatrix4x4& mvp)
	{
		m_meshShader.bind();
		m_meshShader.setUniformValue("mvp_matrix", mvp);
		int vp = m_meshShader.attributeLocation("a_position");
		
		m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(m_vertices));

		m_vertices.bind();
		m_indices.bind();

		glVertexAttribPointer(vp, 3, GL_FLOAT, false, 3 * sizeof(float), NULL);
		glDrawElements(GL_TRIANGLES, m_indices.size() / sizeof(uint32_t),	GL_UNSIGNED_INT, NULL);

		m_vertices.release();
		m_indices.release();

		// Clean up state machine
		m_meshShader.disableAttributeArray(vp);
		m_meshShader.release();
	}


private:
	QOpenGLShaderProgram  m_meshShader;
    QOpenGLBuffer m_vertices;
	QOpenGLBuffer m_indices;
	
};

#endif // GLMESH_H
