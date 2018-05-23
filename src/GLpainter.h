#pragma once
#include <QOpenGLFunctions>
#include <qopenglshaderprogram.h>
#include "Q3d/ObjGLpainter.h"
#include "Q3d/QLine3D.h"
#include <deque>
#include <QPainter>
#include "Params.h"
class GLpainter: protected QOpenGLFunctions
{
    QOpenGLShaderProgram m_lineShader;
    std::vector<ObjGLpainter<QLine3D>> m_drawLines;
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
        m_drawLines.push_back(ObjGLpainter<QLine3D>(l));
    }
    void popDrawLine()
    {
        if (m_drawLines.empty())
            return;
        m_drawLines.pop_back();
    }
    bool init(QPaintDevice * parent)
    {
        m_parent = parent;

        if (!initShader("linev", "linef"))
            return false;
        return true;
    }

    void draw(const QMatrix4x4& mvp)
    {
       
        for (int i = 0; i != m_drawLines.size(); ++i)
        {
            m_lineShader.bind();

            m_lineShader.setUniformValue("mvp_matrix", mvp);
            m_drawLines[i].bind();

            int vp = m_lineShader.attributeLocation("a_xyz");
            m_lineShader.enableAttributeArray(vp);
            m_lineShader.setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(QVector3D));

            m_lineShader.setUniformValue("u_col", m_drawLines[i].getColor());
            m_drawLines[i].draw();

            m_drawLines[i].release();

            // Clean up state machine
            m_lineShader.disableAttributeArray(vp);
            m_lineShader.release();
        }
        if (m_parent)
        {

              
            static const QFont font("Courier", 8);
            static const int fh = QFontMetrics(font).height()+3;
                
            QPainter painter(m_parent);
            painter.setPen(Qt::gray);
            painter.setFont(font);
            painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
            for (int i = 0; i != m_status.size(); ++i)
            painter.drawText(10, m_parent->height()-10-fh*i, *(m_status.end()-1-i));
            painter.end();
        }
      
    }



    void setStatus(const QVector3D& v)
    {
        QString s = QString::number(v[0]) + "," + QString::number(v[1]) + "," + QString::number(v[2]);
        setStatus(s);
    }
    void setStatus(const QString& s)
    {
        m_status.push_front(s);
        m_status.pop_back();
    }





private:
    GLpainter():m_status(Params::nStatusLines()), m_parent(nullptr){}                    // Constructor (the {} brackets) are needed here.


};