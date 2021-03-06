#include "drawable_mesh.h"

DrawableMesh::DrawableMesh(const std::string &name) : Mesh(name),  m_iBuff(QOpenGLBuffer::IndexBuffer) {}
Types::Shape DrawableMesh::getShape() const {	return *this;};
DrawableMesh::~DrawableMesh()
{
	if (m_vBuff.isCreated())
		m_vBuff.destroy();
	if (m_iBuff.isCreated())
		m_iBuff.destroy();
}
const std::string& DrawableMesh::getName() const{return m_name;};
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

void DrawableMesh::paintGL(const QMatrix4x4 &mvp)
{
	int txt = Params::drawablesTexture();
	if (!m_vBuff.isCreated())
		initializeGL();
	if (!m_active)
		return;
	m_vBuff.bind();
	m_iBuff.bind();

	m_meshShader.bind();
	m_meshShader.setUniformValue("mvp_matrix", mvp);
	m_meshShader.setUniformValue("u_txt", txt);
	m_meshShader.setUniformValue("u_lightDir",Params::lightDir());

	int vp = m_meshShader.attributeLocation("a_xyz");
	m_meshShader.enableAttributeArray(vp);
	m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(Types::VertData));

	int vc = m_meshShader.attributeLocation("a_rgb");
	m_meshShader.enableAttributeArray(vc);
	m_meshShader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(Types::VertData));

	

	//glVertexAttribPointer(vc, 3, GL_UNSIGNED_INT8_NV, false, 3 * sizeof(uint8_t), NULL);
	glDrawElements(GL_TRIANGLES, m_iBuff.size() / sizeof(uint32_t), GL_UNSIGNED_INT, NULL);

	m_vBuff.release();
	m_iBuff.release();

	// Clean up state machine
	m_meshShader.disableAttributeArray(vp);
	m_meshShader.disableAttributeArray(vc);
}

std::pair<float, Types::VertData> sprivRayTriangleIntersection(const QVector3D &rp, const QVector3D &rn, const Types::VertData &v0, const Types::VertData &v1, const Types::VertData &v2)
{
	
	static const auto inf = std::make_pair(std::numeric_limits<float>::infinity(), Types::VertData());
	QVector3D v0_(v0);
	QVector3D u = v1 - v0_;
	QVector3D v = v2 - v0_;
	QVector3D n = QVector3D::crossProduct(u, v).normalized();
	if (n.lengthSquared() == 0) // triangle is degenerate
		return inf;				// do not deal with this case

	float b = QVector3D::dotProduct(n, rn);
	if (b == 0)
	{ // ray is  parallel to triangle plane
		return inf;
	}
	float a = QVector3D::dotProduct(n, v0-rp);
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
	QVector3D w = pt - v0_;
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
	std::array<float,3> dist {(pt-v0_).length(),(pt-v1).length(),(pt-v2).length()};
	float m = *std::max_element(dist.begin(),dist.end());
	std::for_each(dist.begin(),dist.end(),[&m](float& v){v=m-v;});
	float distSum = std::accumulate(dist.begin(),dist.end(),0.0f);
	std::for_each(dist.begin(),dist.end(),[&distSum](float& v){v=v/distSum;});
	QVector4D col = QVector4D(v0)*dist[0]+QVector4D(v1)*dist[1]+QVector4D(v2)*dist[2];
	return std::make_pair(r, Types::VertData{pt,col});
}

Types::VertData DrawableMesh::picking(const QVector3D &p, const QVector3D &n) const
{
	float r = std::numeric_limits<float>::infinity();
	static const float inf = std::numeric_limits<float>::infinity();
	Types::VertData pt(inf, inf, inf,0,0,0);
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


bool DrawableMesh::updateVertexBuffer(const Types::VertData* data,size_t n)
{
	std::copy(data,data+n,m_v.begin());
	return DrawableBase::updateVertexBuffer(data,n);
}