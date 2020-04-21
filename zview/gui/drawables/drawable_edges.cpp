#include "drawable_edges.h"


DrawableEdges::DrawableEdges(const std::string& name) :DrawableBase(name), m_eBuff(QOpenGLBuffer::IndexBuffer) {}

DrawableEdges::~DrawableEdges()
{
    if (m_vBuff.isCreated())
        m_vBuff.destroy();
    if (m_eBuff.isCreated())
        m_eBuff.destroy();
}
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

void DrawableEdges::paintGL(const QMatrix4x4& mvp, int txt)
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
        auto xmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData& a, const Types::VertData& b) {return a.x < b.x; });
        auto ymm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData& a, const Types::VertData& b) {return a.y < b.y; });
        auto zmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData& a, const Types::VertData& b) {return a.z < b.z; });
        return Types::Roi3d(xmm.first->x, xmm.second->x, ymm.first->y, ymm.second->y, zmm.first->z, zmm.second->z);

    }
}

float sprivRayEdgeIntersection(const QVector3D& rp, const QVector3D& rn, const QVector3D& v0, const QVector3D& v1)
{
    static const float inf = std::numeric_limits<float>::infinity();
    //todo
    return inf;

}

QVector3D DrawableEdges::picking(const QVector3D& p, const QVector3D& n) const
{
    float r = std::numeric_limits<float>::infinity();
    static const float inf = std::numeric_limits<float>::infinity();
    QVector3D pt(inf, inf, inf);
    for (const auto& i : m_e)
    {
        auto ri = sprivRayEdgeIntersection(p, n, m_v[i[0]], m_v[i[1]]);
        if (r > ri)
        {
            r = ri;


        }
        

    }
    return pt;

}

