#pragma once

#include "QBitFlags.h"
#include "QPrimitive3D.h"
#include <QtGui\qpainterpath.h>
#include <QtGui\qfont.h>


class QText3D : public QPrimitive3D
{
    std::vector<std::vector<QVector3D>> m_poly;
    std::vector<unsigned int> m_npoly;
    QMatrix4x4 m_T;
public:
    QText3D() : QPrimitive3D(QPrimitive3D::Text3D) {};
    QText3D(const QString& str,const QMatrix4x4& m ) : QPrimitive3D(QPrimitive3D::Text3D) { set(str,m); };


    void set(const QString& str,const QMatrix4x4& m)
    {
        QPainterPath path;
        QFont font("Arial", 40);
        path.addText(QPointF(0, 0), QFont("Arial", 40),str);
        QList<QPolygonF> pp = path.toSubpathPolygons();
        m_poly.resize(pp.size());
        float px0 = pp.back().boundingRect().right();
        float py0 = pp.back().boundingRect().bottom();
        float px1 = pp.front().boundingRect().left();
        float s = px1 - px0;
        for (int i = 0; i != pp.size(); ++i)
        {
            m_poly[i].resize(pp[i].size());
            std::transform(pp[i].begin(),pp[i].end(), m_poly[i].begin(), [&](const QPointF& v) {return QVector3D(((v.x()-px0)/s-.5), ((v.y()-py0)/s-.5), 0.0f); });
        }
       /* for (QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++) {
            glBegin(GL_LINE_LOOP);
            for (QPolygonF::iterator p = (*i).begin(); p != i->end(); p++)
                glVertex3f(p->rx()*0.1f, -p->ry()*0.1f, 0);
            glEnd();
        }
        glEnable(GL_LIGHTING);*/
        m_npoly.resize(m_poly.size());
        for (int i = 0; i != m_poly.size(); ++i)
            m_npoly[i] = unsigned int(m_poly[i].size());
        m_T = m;
    }

    QMatrix4x4 getT() const { return m_T; }

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
        unsigned int offset = 0;
        for (int i = 0; i != m_npoly.size(); ++i)
        {
            glDrawArrays(GL_LINE_LOOP, offset , m_npoly[i]);
            offset += m_npoly[i];

        }
       
    }
protected:


};
