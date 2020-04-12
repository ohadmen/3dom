#include "drawable_mesh.h"

DrawableMesh::DrawableMesh(const std::string &name) : DrawableBase(name), m_vBuff(QOpenGLBuffer::VertexBuffer), m_iBuff(QOpenGLBuffer::IndexBuffer) {}

DrawableMesh::~DrawableMesh()
{
	if (m_vBuff.isCreated())
		m_vBuff.destroy();
	if (m_iBuff.isCreated())
		m_iBuff.destroy();
}
void DrawableMesh::initializeGL()
{
	initializeOpenGLFunctions();
	m_vBuff.create();
	m_iBuff.create();
	m_vBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
	m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);

	m_vBuff.bind();

	m_vBuff.allocate(m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
	m_vBuff.write(0, m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
	m_vBuff.release();

	m_iBuff.bind();
	m_iBuff.allocate(m_f.data(), int(m_f.size() * sizeof(Types::FaceIndx)));
	m_iBuff.write(0, m_f.data(), int(m_f.size() * sizeof(Types::FaceIndx)));
	m_iBuff.release();

	privInitShader("mesh");
}

void DrawableMesh::paintGL(const QMatrix4x4 &mvp, int txt)
{
	if (!m_vBuff.isCreated())
		initializeGL();
	if (!m_active)
		return;
	m_vBuff.bind();
	m_iBuff.bind();

	m_meshShader.bind();
	m_meshShader.setUniformValue("mvp_matrix", mvp);

	int vp = m_meshShader.attributeLocation("a_xyz");
	m_meshShader.enableAttributeArray(vp);
	m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(Types::VertData));

	int vc = m_meshShader.attributeLocation("a_rgb");
	m_meshShader.enableAttributeArray(vc);
	m_meshShader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(Types::VertData));

	m_meshShader.setUniformValue("u_txt", txt);

	//glVertexAttribPointer(vc, 3, GL_UNSIGNED_INT8_NV, false, 3 * sizeof(uint8_t), NULL);
	glDrawElements(GL_TRIANGLES, m_iBuff.size() / sizeof(uint32_t), GL_UNSIGNED_INT, NULL);

	m_vBuff.release();
	m_iBuff.release();

	// Clean up state machine
	m_meshShader.disableAttributeArray(vp);
	m_meshShader.disableAttributeArray(vc);
}

std::pair<float, QVector3D> sprivRayTriangleIntersection(const QVector3D &rp, const QVector3D &rn, const QVector3D &v0, const QVector3D &v1, const QVector3D &v2)
{

	static const auto inf = std::make_pair(std::numeric_limits<float>::infinity(), QVector3D());

	QVector3D u = v1 - v0;
	QVector3D v = v2 - v0;
	QVector3D n = QVector3D::crossProduct(u, v).normalized();
	if (n.lengthSquared() == 0) // triangle is degenerate
		return inf;				// do not deal with this case

	float b = QVector3D::dotProduct(n, rn);
	if (b == 0)
	{ // ray is  parallel to triangle plane
		return inf;
	}
	float a = -QVector3D::dotProduct(n, rp - v0);
	// get intersect point of ray with triangle plane
	float r = a / b;
	if (r < 0.0)	// ray goes away from triangle
		return inf; // => no intersect
					// for a segment, also test if (r > 1.0) => no intersect

	QVector3D pt = rp + rn * r; // intersect point of ray and plane

	// is I inside T?

	float uu = QVector3D::dotProduct(u, u);
	float uv = QVector3D::dotProduct(u, v);
	float vv = QVector3D::dotProduct(v, v);
	QVector3D w = pt - v0;
	float wu = QVector3D::dotProduct(w, u);
	float wv = QVector3D::dotProduct(w, v);
	float d = uv * uv - uu * vv;

	// get and test parametric coords
	float s;
	s = (uv * wv - vv * wu) / d;
	if (s < 0.0 || s > 1.0) // I is outside T
	{
		return inf;
	}
	float t = (uv * wu - uu * wv) / d;
	if (t < 0.0 || (s + t) > 1.0) // I is outside T
	{
		return inf;
	}
	return std::make_pair(r, pt);
}

QVector3D DrawableMesh::picking(const QVector3D &p, const QVector3D &n) const
{
	float r = std::numeric_limits<float>::infinity();
	static const float inf = std::numeric_limits<float>::infinity();
	QVector3D pt(inf, inf, inf);
	for (const auto &i : m_f)
	{
		auto rp = sprivRayTriangleIntersection(p, n, m_v[i[0]], m_v[i[1]], m_v[i[2]]);
		if (r > rp.first)
		{
			r = rp.first;
			pt = rp.second;
		}
	}

	return pt;
}
Types::Roi3d DrawableMesh::get3dbbox() const {return Types::Mesh::get3dbbox();}