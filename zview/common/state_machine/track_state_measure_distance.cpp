#include "libcurie/drawables/drawable_basic_shapes.h"
#include "track_state_measure_distance.h"
#include "libcurie/common/vp_mat.h"
#include "track_state_common.h"
#include <cmath>
#include <sstream>

TrackStateMeasureDistance::TrackStateMeasureDistance() 
{
    drawableBasicShapes.remove(drawble_line_key);
    drawableBasicShapes.remove(drawble_p0_key);
    drawableBasicShapes.remove(drawble_p1_key);
    
}

bool TrackStateMeasureDistance::setMesuringStartPoint(const QPointF& xy)
{
    m_p0 = TrackStateCommon::selectPointFromScreen(xy);
    bool ok = !std::isinf(m_p0[0]);
    if (!ok)
        return false;
    drawableBasicShapes.insert(drawble_p0_key, m_p0, { 1,0,0 });
    //canvasUpdate();
    return true;
}
void TrackStateMeasureDistance::input(QKeyEvent* e)
{
   if (e->type() == QInputEvent::KeyPress && e->modifiers() == Qt::KeyboardModifier::NoModifier && e->key() == Qt::Key::Key_D)
    {
       QVector3D p1 = TrackStateCommon::selectPointFromScreen(m_mousepos);
       if (!std::isinf(p1[0]))
       {
           QVector3D d = (p1 - m_p0);
           std::stringstream ss;
           ss << "distance:"<< d.length() << " ["<<d.x() << ", " << d.y() << ", " << d.z() << "]";
           setStatus(ss.str());
           drawableBasicShapes.insert(drawble_p1_key, m_p0, { 0,1,0 });
           drawableBasicShapes.insert(drawble_line_key, { m_p0,p1 }, { 0,0,255 });
           canvasUpdate();
       }
       m_machineP->setState(new TrackStateIdle);
    }
   else
       TrackStateIdle::input(e);

    //QVector3D x = TrackStateCommon::selectPointFromScreen(e->localPos());

}
