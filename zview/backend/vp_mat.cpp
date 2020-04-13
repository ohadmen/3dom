#include "vp_mat.h"
#include "zview/common/params.h"

VPmat::VPmat() : m_w(1), m_h(1)
{
    m_proj.setToIdentity();
    m_view.setToIdentity();
}


QVector3D VPmat::xy2screen(const QPointF &xy) const
{
    QVector4D uv(xy.x() / m_w * 2 - 1, -(xy.y() / m_h * 2 - 1), 1, 1);
    QVector4D sv = m_proj.inverted() * uv;
    return QVector3D(sv);
}

QMatrix4x4 VPmat::operator()() const { return m_proj * m_view; }
void VPmat::setWinSize(int w, int h)
{
    m_w = w;
    m_h = h;
    float ar = float(w) / h;
    m_proj.setToIdentity();
    m_proj.perspective(Params::camFOV(), ar, Params::camZnear(), Params::camZfar());
}

const QMatrix4x4 &VPmat::getViewMatrix() const { return m_view; }

void VPmat::setViewMatrix(const QMatrix4x4 &m) { m_view = m; }

QMatrix4x4 VPmat::rotate(float a, const QVector3D &v)
{
    QMatrix4x4 m;
    m.rotate(a, v);
    return m;
}
QMatrix4x4 VPmat::scale(float v)
{
    QMatrix4x4 m;
    m.scale(v);
    return m;
}
QMatrix4x4 VPmat::translate(const QVector3D &v)
{
    QMatrix4x4 m;
    m.translate(v);
    return m;
}

const std::pair<QVector3D, QVector3D> VPmat::xy2ray(const QPointF &xy)const
{
    QMatrix4x4 vm = getViewMatrix();
    QMatrix4x4 vmI = vm.inverted();
    QVector3D p = vmI * QVector3D(0, 0, 0);
    QVector3D p2 = vmI * xy2screen(xy);
    QVector3D n = (p2 - p).normalized();
    return std::make_pair(p, n);
}

