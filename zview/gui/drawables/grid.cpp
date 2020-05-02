#include "grid.h"
#include "zview/common/common_types.h"
#include "zview/common/params.h"
#include "drawable_common.h"
#include <QtGui/QMatrix4x4>
#include <QtGui/QVector4D>
#include  <cmath>

Grid::Grid():m_scale(10) {}
void Grid::setScale(float s) { m_scale = s; }

void Grid::initializeGL()
{
    Q_INIT_RESOURCE(shaders);
    initializeOpenGLFunctions();
    DrawableCommon::initShadar(&m_shader, "edges");
    constexpr int n2 = n/2;
    static uint8_t col = 255;
    static uint8_t  alpha = 50;
    std::array<Types::VertData, n * 4> vbuf;
    for (int i = 0; i != n; ++i)
    {
        vbuf[2 * i + 0] = Types::VertData(i - n2, +n2, 0, col, col, col,alpha);
        vbuf[2 * i + 1] = Types::VertData(i - n2, -n2, 0, col, col, col,alpha);
        vbuf[2 * i + 0 + 2 * n] = Types::VertData(n2, i - n / 2, 0, col, col, col,alpha);
        vbuf[2 * i + 1 + 2 * n] = Types::VertData(-n2, i - n / 2, 0, col, col, col,alpha);
    }
    if (m_verts.isCreated())
        m_verts.destroy();

    m_verts.create();
    m_verts.bind();
    m_verts.allocate(vbuf.data(), sizeof(vbuf));
    m_verts.release();
}

void Grid::paintGL(const QMatrix4x4 &mvp)
{
    QVector3D p = mvp.inverted()*QVector3D(0, 0, 0);
    QVector3D p1 = mvp.inverted()*QVector3D(0, 0, 1);
    QVector3D l = (p1-p).normalized();
    if(fabs(l[2])<1e-4)
        return;
    QVector3D pFloor = p-p[2]/l[2]*l;
    QMatrix4x4 localmvp = mvp;

    auto r = mvp.row(0);
    r.setW(0);
    float s = 1/r.length();
    float q = float(std::pow(10,int(0.85*std::log10(100*s))))/100;
    
    pFloor[0] = int(pFloor[0]/q);
    pFloor[1] = int(pFloor[1]/q);


    // float f = 10;
    // // float s = 
    
    // float ox = localmvp.data()[12]*s;
    // float oy = localmvp.data()[13]*s;
    localmvp.scale(q);
    localmvp.translate(pFloor);
    

    
    
    
    // localmvp.scale(s);
    


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

    glDrawArrays(GL_LINES, 0, 4*n);

    m_verts.release();
    m_shader.release();
}
