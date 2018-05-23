#pragma once

#include "QBitFlags.h"
#include "QPrimitive3D.h"
#include <QtGui\qpainterpath.h>
#include <QtGui\qfont.h>


class QText3D : public QPrimitive3D
{
public:
    QText3D ();

    virtual ~QText3D();
    void set(const QString& str)
    {
        QPainterPath path;
        QFont font("Arial", 40);
        path.addText(QPointF(0, 0), QFont("Arial", 40), QString(QObject::tr("This is a test")));
        QList<QPolygonF> poly = path.toSubpathPolygons();
       /* for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++) {
            glBegin(GL_LINE_LOOP);
            for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
                glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0);
            glEnd();
        }
        glEnable(GL_LIGHTING);*/
    }
protected:


};
