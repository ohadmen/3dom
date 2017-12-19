#ifndef GLMESH_H
#define GLMESH_H

#include <QtOpenGL/QGLBuffer>
#include <QtOpenGL/QGLFunctions>
#include "mesh.h"

class GLMesh : protected QGLFunctions
{
public:
	GLMesh() : m_vertices(QGLBuffer::VertexBuffer), m_indices(QGLBuffer::IndexBuffer)
	{

	}
	void glinit()
	{
		
		initializeGLFunctions();
		m_meshShader.addShaderFromSourceFile(QGLShader::Vertex, ":/gl/mesh.vert");
		m_meshShader.addShaderFromSourceFile(QGLShader::Fragment, ":/gl/mesh.frag");
		m_meshShader.link();
	}
	void set(const Mesh&  mesh)
		
	{
		initializeGLFunctions();

		m_vertices.create();
		m_indices.create();

		m_vertices.setUsagePattern(QGLBuffer::StaticDraw);
		m_indices.setUsagePattern(QGLBuffer::StaticDraw);

		m_vertices.bind();
		m_vertices.allocate(mesh.vertData(),	int(mesh.vertSize() * sizeof(float)*3));
		m_vertices.release();

		m_indices.bind();
		m_indices.allocate(mesh.indData(),int(mesh.indSize() * sizeof(uint32_t)));
		m_indices.release();
		
	}
    void draw(const QMatrix4x4& transformMatrix, const QMatrix4x4& viewMatrix)
	{
		m_meshShader.bind();
		glUniformMatrix4fv(
			m_meshShader.uniformLocation("transform_matrix"),
			1, GL_FALSE, transformMatrix.data());

		glUniformMatrix4fv(
			m_meshShader.uniformLocation("view_matrix"),
			1, GL_FALSE, viewMatrix.data());
		const GLuint vp = m_meshShader.attributeLocation("vertex_position");
		glEnableVertexAttribArray(vp);

		m_vertices.bind();
		m_indices.bind();

		glVertexAttribPointer(vp, 3, GL_FLOAT, false, 3 * sizeof(float), NULL);
		glDrawElements(GL_TRIANGLES, m_indices.size() / sizeof(uint32_t),	GL_UNSIGNED_INT, NULL);
		




		m_vertices.release();
		m_indices.release();

		// Clean up state machine
		glDisableVertexAttribArray(vp);
		m_meshShader.release();
	}


private:
	QGLShaderProgram m_meshShader;
    QGLBuffer m_vertices;
    QGLBuffer m_indices;
	
};

#endif // GLMESH_H
