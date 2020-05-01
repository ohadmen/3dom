#include "axes.h"
#include "zview/common/common_types.h"
#include "zview/common/params.h"
#include "drawable_common.h"
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector4D>

Axes::Axes() : m_scale(1) {}

void Axes::initializeGL()
{
    Q_INIT_RESOURCE(shaders);
    initializeOpenGLFunctions();
    DrawableCommon::initShadar(&m_shader, "edges");

    std::array<Types::VertData, 6> vbuf;
    vbuf = {
        Types::VertData(0, 0, 0, 255, 000, 000),
        Types::VertData(1, 0, 0, 255, 000, 000),
        Types::VertData(0, 0, 0, 000, 255, 000),
        Types::VertData(0, 1, 0, 000, 255, 000),
        Types::VertData(0, 0, 0, 000, 000, 255),
        Types::VertData(0, 0, 1, 000, 000, 255)};
    if (m_verts.isCreated())
        m_verts.destroy();

    m_verts.create();
    m_verts.bind();
    m_verts.allocate(vbuf.data(), sizeof(vbuf));
    m_verts.release();
}
void Axes::setScale(float s) { m_scale = s; }

void Axes::paintGL(const QMatrix4x4 &mvp)
{

    QMatrix4x4 localmvp = mvp;

    localmvp.scale(m_scale);

    m_shader.bind();
    m_verts.bind();

    m_shader.setUniformValue("mvp_matrix", localmvp);
    m_shader.setUniformValue("u_txt", 1);

    int vp = m_shader.attributeLocation("a_xyz");
    m_shader.enableAttributeArray(vp);
    m_shader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(Types::VertData));

    int vc = m_shader.attributeLocation("a_rgb");
    m_shader.enableAttributeArray(vc);
    m_shader.setAttributeBuffer(vc, GL_UNSIGNED_BYTE, 3 * sizeof(float), 4, sizeof(Types::VertData));

    glDrawArrays(GL_LINES, 0, 6);

    m_verts.release();
    m_shader.release();
}
