#include "drawable_edges.h"
#include <QtGui/QMatrix2x2>
#include <cmath>

DrawableEdges::DrawableEdges(const std::string &name) : Edges(name), m_eBuff(QOpenGLBuffer::IndexBuffer) {}
Types::Shape DrawableEdges::getShape() const {	return *this;};
DrawableEdges::~DrawableEdges()
{
    if (m_vBuff.isCreated())
        m_vBuff.destroy();
    if (m_eBuff.isCreated())
        m_eBuff.destroy();
}
const std::string& DrawableEdges::getName() const{return m_name;};
void DrawableEdges::initializeGL()
{
    initializeOpenGLFunctions();
    m_vBuff.create();
    m_eBuff.create();
    m_vBuff.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_eBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);

    m_vBuff.bind();

    m_vBuff.allocate(m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
    m_vBuff.write(0, m_v.data(), int(m_v.size() * sizeof(Types::VertData)));
    m_vBuff.release();

    m_eBuff.bind();
    m_eBuff.allocate(m_e.data(), int(m_e.size() * sizeof(Types::EdgeIndx)));
    m_eBuff.write(0, m_e.data(), int(m_e.size() * sizeof(Types::EdgeIndx)));
    m_eBuff.release();

    privInitShader("edges");
}

void DrawableEdges::paintGL(const QMatrix4x4 &mvp, int txt)
{
    if (!m_vBuff.isCreated())
        initializeGL();
    if (!m_active)
        return;

    m_vBuff.bind();
    m_eBuff.bind();

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
    glDrawElements(GL_LINES, m_eBuff.size() / sizeof(uint32_t), GL_UNSIGNED_INT, NULL);

    m_vBuff.release();
    m_eBuff.release();

    // Clean up state machine
    m_meshShader.disableAttributeArray(vp);
    m_meshShader.disableAttributeArray(vc);
}

Types::Roi3d DrawableEdges::get3dbbox() const
{
    if (m_v.size() == 0)
    {
        static const float e = 1e-3;
        return Types::Roi3d(-e, e, -e, e, -e, e);
    }
    else if (m_v.size() == 1)
    {
        static const float e = 1e-3;
        return Types::Roi3d(m_v[0].x - e, m_v[0].x + e, m_v[0].y - e, m_v[0].y + e, m_v[0].z - e, m_v[0].z + e);
    }
    else
    {
        auto xmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData &a, const Types::VertData &b) { return a.x < b.x; });
        auto ymm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData &a, const Types::VertData &b) { return a.y < b.y; });
        auto zmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData &a, const Types::VertData &b) { return a.z < b.z; });
        return Types::Roi3d(xmm.first->x, xmm.second->x, ymm.first->y, ymm.second->y, zmm.first->z, zmm.second->z);
    }
}

QVector3D DrawableEdges::picking(const QVector3D &p1, const QVector3D &n1) const
{
    static const float angularthresholt = std::tan(2.0 * M_PI / 180.0);
    static const float inf = std::numeric_limits<float>::infinity();

    float closestDistance = inf;
    QVector3D closestPoint(inf, inf, inf);
    for (const auto &i : m_e)
    {
        const QVector3D &p2 = m_v[i[0]];
        const QVector3D &p2_ = m_v[i[1]];
        
        
        QVector3D n2 = (p2_ - p2).normalized();
        QVector3D q = p1 - p2;
        float c = QVector3D::dotProduct(n1, n2);
        
        float invdet = 1.0 / (-1.0 + c * c);
        float n1q = QVector3D::dotProduct(n1, q);
        float n2q = QVector3D::dotProduct(n2, q);
        float mu1 = (1 * n1q - c * n2q) * invdet;
        float mu2 = (c * n1q - 1 * n2q) * invdet;
        QVector3D pl1 = p1 + mu1 * n1;
        QVector3D pl2 = p2 + mu2 * n2;
        float d = pl1.length();
        if (d > closestDistance)
            continue;
        float a = (pl2 - pl1).length();
        if (a / d > angularthresholt)
            continue;
        closestDistance = d;
        closestPoint = pl2;
    }
    return closestPoint;
}
