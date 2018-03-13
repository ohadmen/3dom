#pragma once
#include <Qt>
#include <QOpenGLFunctions>
#include <Q3d/QPrimitive3D.h>
template<class Obj>
class ObjGLpainter : protected QOpenGLFunctions
{
    Obj m_obj;
    QVector4D m_color;
    QOpenGLBuffer m_buff;

public:
    ObjGLpainter(const Obj& obj=Obj()):m_obj(obj), m_color(1,1,1,1)
    {
        (void)static_cast<QPrimitive3D*>((Obj*)0);//make sure base class is QPrimitive3D
        init();
    }
    QVector4D getColor()const { return m_color; }
    void bind()
    {
        m_buff.bind();
    }
    void draw()
    {
        initializeOpenGLFunctions();
        
       



        glLineWidth(3);
        glDrawArrays(GL_LINE_LOOP, 0, m_buff.size() / sizeof(QVector3D));

     
       
    }
    void release()
    {
        m_buff.release();
    }
    void init()
    {
        m_buff.create();
        m_buff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_buff.bind();
        switch (m_obj.type())
        {
        case QPrimitive3D::Line3D:
        {
            QVector3D data[2];
            data[0] = m_obj.p1();
            data[1] = m_obj.p2();
            m_buff.allocate(data, 2 * sizeof(QVector3D));
            m_buff.release();
            break;
        }
        //case QPrimitive3D::Circle:
        //    static const int nPoints = 64;
        //    static const float twopi = std::acos(0) * 4;
        //    static const float radius = 1;
        //    std::vector<QVector3D> data(nnPoints);
        //    for (int i = 0; i != nPoints; ++i)
        //    {
        //        data[i] = QVector3D(radius*std::cos(i * twopi / nPoints), radius*std::sin(i * twopi / nPoints), 0.0, 1, 0, 0);
        //    }
        //    m_buff.allocate(data.data(), nPoints * sizeof(QVector3D));
        //    m_buff.release();

        }
    }
   
};
