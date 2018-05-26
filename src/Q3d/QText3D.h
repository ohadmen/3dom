#pragma once

#include "QBitFlags.h"
#include "QPrimitive3D.h"
#include <QtGui\qpainterpath.h>
#include <QtGui\qfont.h>


class QText3D : public QPrimitive3D
{
    QList<QPolygonF> m_poly;
    std::vector<unsigned int> m_npoly;
public:
    QText3D() : QPrimitive3D(QPrimitive3D::Text3D) {};
    QText3D(const QString& str) : QPrimitive3D(QPrimitive3D::Text3D) { set(str); };


    void set(const QString& str)
    {
        QPainterPath path;
        QFont font("Arial", 40);
        path.addText(QPointF(0, 0), QFont("Arial", 40),str);
        m_poly = path.toSubpathPolygons();
       /* for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++) {
            glBegin(GL_LINE_LOOP);
            for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
                glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0);
            glEnd();
        }
        glEnable(GL_LIGHTING);*/
        m_npoly.resize(m_poly.size());
        for (int i = 0; i != m_poly.size(); ++i)
            m_npoly[i] = m_poly[i].size();
        
    }
    const QPolygonF& poly(int i)const { return m_poly[i]; }

    void init()
    {

        m_buff.create();
        m_buff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_buff.bind();
        unsigned int ntot = std::accumulate(m_npoly.begin(), m_npoly.end(), unsigned int(0));
        std::vector<QVector3D> dataFlat; dataFlat.reserve(ntot);
        for (int i = 0; i != m_npoly.size(); ++i)
            dataFlat.insert(dataFlat.end(),m_poly[i].begin(), m_poly[i].end());
        m_buff.allocate(dataFlat.data(), ntot * sizeof(QVector3D));
        m_buff.release();

    }
    void draw()
    {
        initializeOpenGLFunctions();

        //glMultiDrawArrays(GL_LINE_LOOP, 0, &npoly[0], npoly.size());
        int offset = 0;
        for (int i = 0; i != m_npoly.size(); ++i)
        {
            glDrawArrays(GL_LINE_LOOP, offset, m_npoly[i] / sizeof(QVector3D));
            offset += m_npoly[i];
        }
    }
protected:


};
