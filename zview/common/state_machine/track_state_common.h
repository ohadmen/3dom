#pragma once
#include "libcurie/common/vp_mat.h"
#include "libcurie/drawables/drawables_buffer.h"
#include <QtCore/QPointF>

namespace TrackStateCommon
{
    static QVector3D selectPointFromScreen(const QPointF& xy)
    {
        QMatrix4x4 vm = vpmat.getViewMatrix();
        QMatrix4x4 vmI = vm.inverted();
        QVector3D  p = vmI * QVector3D(0, 0, 0);
        QVector3D p2 = vmI * vpmat.xy2screen(xy);
        QVector3D n = (p2 - p).normalized();
        QVector3D x = drawablesBuffer.getRayIntersection(p, n);
        return x;

}
}