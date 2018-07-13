#pragma once
#include <deque>
#include <tuple>


#include <QOpenGLFunctions>
#include <qopenglshaderprogram.h>
#include <QPainter>
#include "Qmvp.h"
#include "Q3d/QLine3D.h"
#include "Params.h"
class GLpainter: protected QOpenGLFunctions
{
    QOpenGLShaderProgram m_lineShader;
    std::vector<QLine3D> m_drawLines;
    std::vector<std::tuple<QString,QVector3D,QVector4D>> m_drawText;
    std::deque<QString> m_status;
    QPaintDevice * m_parent;
public:

    static GLpainter& i()
    {
        static GLpainter    instance; // Guaranteed to be destroyed.

        return instance;
    }
    bool initShader(const QString& vshader, const QString& fshader)
    {
        // Compile vertex shader
        if (!m_lineShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/" + vshader + ".glsl"))
            return false;

        // Compile fragment shader
        if (!m_lineShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/" + fshader + ".glsl"))
            return false;

        // Link shader pipeline
        if (!m_lineShader.link())
            return false;
        return true;

    }
    void addDrawLine(const QLine3D& l)
    {
        m_drawLines.push_back(l);
        m_drawLines.back().init();
    }
    size_t nDrawLines()const  { return m_drawLines.size(); }
    void popDrawLine()
    {
        if (m_drawLines.empty())
            return;
        m_drawLines.pop_back();
    }
    void popDrawText()
    {
        if (m_drawText.empty())
            return;
        m_drawText.pop_back();
    }
    void addDrawText(const QString& s, const QVector3D & t, const QVector4D & c)
    {
        m_drawText.emplace_back(std::make_tuple(s, t,c));
        
    }
    bool init(QPaintDevice * parent)
    {
        m_parent = parent;

        if (!initShader("linev", "linef"))
            return false;
        return true;
    }

    void draw(const Qmvp& mvp)
    {
       //drawlines
        
        for (int i = 0; i != m_drawLines.size(); ++i)
        {
            m_lineShader.bind();

            m_lineShader.setUniformValue("mvp_matrix", mvp.getMat());
            m_drawLines[i].bind();

            int vp = m_lineShader.attributeLocation("a_xyz");
            m_lineShader.enableAttributeArray(vp);
            m_lineShader.setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(QVector3D));

            m_lineShader.setUniformValue("u_col", m_drawLines[i].getColor());
            m_lineShader.setUniformValue("u_doVarCol",0.0f);
            
            m_drawLines[i].draw();

            m_drawLines[i].release();

            // Clean up state machine
            m_lineShader.disableAttributeArray(vp);
            m_lineShader.release();
        }

       
        if (m_parent)
        {

              
            static const QFont font("Courier", 12);
            static const int fh = QFontMetrics(font).height()+3;
                
            QPainter painter(m_parent);
            painter.setPen(Qt::gray);
            painter.setFont(font);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            for (int i = 0; i != m_status.size(); ++i)
            {
                
                painter.setPen(Qt::black);
                painter.drawText(10, m_parent->height() - 10 - fh*i, *(m_status.end() - 1 - i));
                painter.setPen(Qt::gray);
                painter.drawText(11, m_parent->height() - 9 - fh*i, *(m_status.end() - 1 - i));
            }
            

            //drawText
            
            for (int i = 0; i != m_drawText.size(); ++i)
            {
                 QVector4D c = std::get<2>(m_drawText[i])*255;
                painter.setPen(QColor(c[0],c[1],c[2]));
                QVector2D uv = mvp.project(std::get<1>(m_drawText[i]));
               

                painter.setPen(Qt::black);
                painter.drawText(QPointF(uv.x() + 1, uv.y() + 1 + fh), std::get<0>(m_drawText[i]));

                painter.setPen(QColor(c[0], c[1], c[2]));
                painter.drawText(QPointF(uv.x(),uv.y()+fh), std::get<0>(m_drawText[i]));

            }

            painter.end();
        }
      
    }



    void setStatus(const QVector3D& v)
    {
        QString s = QString::number(v[0]) + "," + QString::number(v[1]) + "," + QString::number(v[2]);
        setStatus(s);
    }
    void setStatus(const QString& s,bool newline=true)
    {
        if (newline)
        {
            m_status.push_front(s);
            m_status.pop_back();
        }
        else
        {
            m_status.front() += s;
        }
        
        
    }





private:
    GLpainter():m_status(Params::nStatusLines()), m_parent(nullptr){}                    // Constructor (the {} brackets) are needed here.


};