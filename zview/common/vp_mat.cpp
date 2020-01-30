#include "vp_mat.h"
//signelon, view projection matrix
#define vpmat VPmat::i()
VPmat::VPmat() :m_w(1), m_h(1)
{
    m_proj.setToIdentity();
    m_view.setToIdentity();


}
VPmat& VPmat::i()
{
    static VPmat v;
    return v;
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

const QMatrix4x4& VPmat::getProjMatrix() const { return m_proj; }
const QMatrix4x4& VPmat::getViewMatrix() const { return m_view; }

void VPmat::setViewMatrix(const QMatrix4x4& m) { m_view = m; }

QMatrix4x4 VPmat::rotate(float a, const QVector3D& v) { QMatrix4x4 m; m.rotate(a, v); return m; }
QMatrix4x4 VPmat::scale(float       v) { QMatrix4x4 m; m.scale(v); return m; }
QMatrix4x4 VPmat::translate(const QVector3D& v) { QMatrix4x4 m; m.translate(v); return m; }


