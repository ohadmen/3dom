#pragma once

#include <QOpenGLFunctions>
#include <qopenglshaderprogram.h>
#include "Q3d/QLine3D.h"
#include <qopenglbuffer.h>
class MeasurmentGrid: protected QOpenGLFunctions
{
    QOpenGLShaderProgram m_lineShader;
    QOpenGLBuffer m_buff;
    int m_nLines;

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

public:

    bool init()
    {
        if (!initShader("lineV", "lineF"))
            return false;
        static const float z = 0.9f;
        QVector3D data[]=
        { 
            QVector3D(-1.0f,-1.0f,z),QVector3D(-1.0f,1.0f,z),
            QVector3D(-0.8f,-1.0f,z),QVector3D(-0.8f,1.0f,z),
            QVector3D(-0.6f,-1.0f,z),QVector3D(-0.6f,1.0f,z),
            QVector3D(-0.4f,-1.0f,z),QVector3D(-0.4f,1.0f,z),
            QVector3D(-0.2f,-1.0f,z),QVector3D(-0.2f,1.0f,z),
            QVector3D(-0.0f,-1.0f,z),QVector3D(-0.0f,1.0f,z),
            QVector3D( 0.2f,-1.0f,z),QVector3D( 0.2f,1.0f,z),
            QVector3D( 0.4f,-1.0f,z),QVector3D( 0.4f,1.0f,z),
            QVector3D( 0.6f,-1.0f,z),QVector3D( 0.6f,1.0f,z),
            QVector3D( 0.8f,-1.0f,z),QVector3D( 0.8f,1.0f,z),
            QVector3D( 1.0f,-1.0f,z),QVector3D( 1.0f,1.0f,z),

            QVector3D(-1.0f,-1.0f,z),QVector3D(1.0f,-1.0f,z),
            QVector3D(-1.0f,-0.8f,z),QVector3D(1.0f,-0.8f,z),
            QVector3D(-1.0f,-0.6f,z),QVector3D(1.0f,-0.6f,z),
            QVector3D(-1.0f,-0.4f,z),QVector3D(1.0f,-0.4f,z),
            QVector3D(-1.0f,-0.2f,z),QVector3D(1.0f,-0.2f,z),
            QVector3D(-1.0f,-0.0f,z),QVector3D(1.0f,-0.0f,z),
            QVector3D(-1.0f, 0.2f,z),QVector3D(1.0f, 0.2f,z),
            QVector3D(-1.0f, 0.4f,z),QVector3D(1.0f, 0.4f,z),
            QVector3D(-1.0f, 0.6f,z),QVector3D(1.0f, 0.6f,z),
            QVector3D(-1.0f, 0.8f,z),QVector3D(1.0f, 0.8f,z),
            QVector3D(-1.0f, 1.0f,z),QVector3D(1.0f, 1.0f,z)
        };
        m_nLines = sizeof(data) / sizeof(QVector3D);
       
        m_buff.create();
        m_buff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_buff.bind();
        m_buff.allocate(data, m_nLines *sizeof(QVector3D));
        m_buff.release();

        return true;
    }
    void draw( QMatrix4x4 mvp)
    {
        mvp.setToIdentity();
        
        initializeOpenGLFunctions();
        m_lineShader.bind();
        m_buff.bind();
        
        m_lineShader.setUniformValue("mvp_matrix", mvp);


        int vp = m_lineShader.attributeLocation("a_xyz");
        m_lineShader.enableAttributeArray(vp);
        m_lineShader.setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(QVector3D));

        m_lineShader.setUniformValue("u_col", QVector4D(0.5,0.5,0.5,1.0));
        m_lineShader.setUniformValue("u_bcol", QVector4D(0.5, 0.5, 0.5, 1.0));

        glLineWidth(1);
        for(int i=0;i!= m_nLines;++i)
            glDrawArrays(GL_LINE_LOOP, i*2, 2);
        m_buff.release();
        m_lineShader.disableAttributeArray(vp);
        m_lineShader.release();
    }
};
