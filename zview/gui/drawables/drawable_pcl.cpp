#include "drawable_pcl.h"
#include "zview/common/params.h"

DrawablePcl::DrawablePcl(const std::string& name) :DrawableBase(name){}

DrawablePcl::~DrawablePcl()
{
	if (m_vBuff.isCreated())
		m_vBuff.destroy();
}
void DrawablePcl::initializeGL()
{
	initializeOpenGLFunctions();
	m_vBuff.create();
	m_vBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);

	m_vBuff.bind();

	m_vBuff.allocate(m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
	m_vBuff.write(0, m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
	m_vBuff.release();

	privInitShader("meshV");

    

}

void DrawablePcl::paintGL(const QMatrix4x4& mvp, int txt)
{
    if (!m_vBuff.isCreated())
        initializeGL();
    if (!m_active)
        return;

    float s = mvp.column(0).length();

	m_vBuff.bind();
	m_meshShader.bind();
	m_meshShader.setUniformValue("mvp_matrix", mvp);

	m_meshShader.setUniformValue("u_ptSize", Params::pointSize()*s);

	

	int vp = m_meshShader.attributeLocation("a_xyz");
	m_meshShader.enableAttributeArray(vp);
	m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(Types::VertData));

	int vc = m_meshShader.attributeLocation("a_rgb");
	m_meshShader.enableAttributeArray(vc);
	m_meshShader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(Types::VertData));


	m_meshShader.setUniformValue("u_txt", txt);
	//glPointSize(Params::pointSize());
	glDrawArrays(GL_POINTS, 0,m_vBuff.size() / sizeof(float));

	m_vBuff.release();

	// Clean up state machine
	m_meshShader.disableAttributeArray(vp);
	m_meshShader.disableAttributeArray(vc);







}


Types::Roi3d DrawablePcl::get3dbbox() const
{
	auto xmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData& a, const Types::VertData& b) {return a.x < b.x; });
	auto ymm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData& a, const Types::VertData& b) {return a.y < b.y; });
	auto zmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData& a, const Types::VertData& b) {return a.z < b.z; });
	Types::Roi3d q(xmm.first->x, xmm.second->x, ymm.first->y, ymm.second->y, zmm.first->z, zmm.second->z);
	return q;
}



QVector3D DrawablePcl::picking(const QVector3D& p, const QVector3D& n) const
{
    constexpr float inf = std::numeric_limits<float>::infinity();
    QVector3D pt(inf, inf, inf);

	return pt;

}
