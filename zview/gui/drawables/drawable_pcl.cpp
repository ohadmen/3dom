#include <cmath>
#include "drawable_pcl.h"


DrawablePcl::DrawablePcl(const std::string& name) :Pcl(name){}

DrawablePcl::~DrawablePcl()
{
	if (m_vBuff.isCreated())
		m_vBuff.destroy();
}
const std::string& DrawablePcl::getName() const{return m_name;};
void DrawablePcl::initializeGL()
{
	initializeOpenGLFunctions();
	m_vBuff.create();
	m_vBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);

	m_vBuff.bind();

	m_vBuff.allocate(m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
	m_vBuff.write(0, m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
	m_vBuff.release();

	privInitShader("point");

    

}
Types::Shape DrawablePcl::getShape() const {	return *this;};

void DrawablePcl::paintGL(const QMatrix4x4& mvp)
{
    if (!m_vBuff.isCreated())
	{
        initializeGL();
	}
    if (!m_active)
	{
        return;
	}
	int txt = Params::drawablesTexture();
    

	m_vBuff.bind();
	m_meshShader.bind();
	m_meshShader.setUniformValue("mvp_matrix", mvp);

	
	m_meshShader.setUniformValue("u_ptSize", Params::pointSize());
	m_meshShader.setUniformValue("u_lightDir",Params::lightDir());
	m_meshShader.setUniformValue("u_txt", txt);

	int vp = m_meshShader.attributeLocation("a_xyz");
	m_meshShader.enableAttributeArray(vp);
	m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(Types::VertData));

	int vc = m_meshShader.attributeLocation("a_rgb");
	m_meshShader.enableAttributeArray(vc);
	m_meshShader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(Types::VertData));


	
	

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
	static const float angularthresholt = std::tan(2.0*M_PI/180.0);
    constexpr float inf = std::numeric_limits<float>::infinity();
	float closestDistance = inf;
	QVector3D closestPoint(inf,inf,inf);
	for(const auto& pt:m_v)
	{
		auto pt_ = QVector3D(pt);
		float d = QVector3D::dotProduct(n,pt_-p);
		if(d<0)//behind camera
			continue;
		if(closestDistance<d)
			continue;
		float a = (p+n*d-pt_).length();
		if(a/d>angularthresholt)
			continue;
		closestDistance = d;
		closestPoint = pt_;
	}
	return closestPoint;

}
