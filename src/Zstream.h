#pragma once
#include <array>
#include <stdint.h>
#include <vector>
#include <string>
#include <fstream>
#include <map>

#include <Qt>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include "Array2d.h"


class Zsteam : protected QOpenGLFunctions
{
    
    typedef std::vector<unsigned int> IndArr;
    typedef std::vector<std::array<std::uint16_t, 2>> Zdata;
    //--------------INTERNAL TYPE DECL---------------
    struct FrameData
    {
        IndArr indices;
        std::vector<std::array<std::uint16_t, 2>> xy;
        QMatrix3x3 kinv;
        float z2mm;
        uint16_t width;
        uint16_t height;
        
    };
    //--------------DATA MEMBERS---------------
    FrameData m_fd;
    std::vector<float>         m_zdata;
    QOpenGLBuffer m_izBuff; //z
    QOpenGLBuffer m_xyBuff; //z
    QOpenGLBuffer m_iBuff; //indices

    bool m_glInitialized;
    QOpenGLShaderProgram m_meshShader;

    static  IndArr privTriangulate(uint16_t w, uint16_t h)
    {
        uint32_t key = (uint32_t(w) << 16) + h;
        static std::map<uint32_t, IndArr> map;

        if (map.find(key)==map.end())
        {
            IndArr a((w +1)*(h - 1) * 2);
            auto sub2ind = [&w](int xx, int yy) {return xx + yy*w; };
            for (int y = 0; y != h - 1; ++y)
            {
                auto arow= &a[(w+1 ) * 2 * y];
                for (int x = 0; x != w ; ++x)
                {
                    arow[2 * x + 0] = sub2ind(x, y);
                    arow[2 * x + 1] = sub2ind(x, y+1);
                }
                arow[2*w]= sub2ind(w-1, y + 1);
                arow[2 * w + 1] = sub2ind(0, y + 1);
            }
            map.insert(std::make_pair(key, a));
        }
        return map.at(key);
    }
    static QMatrix3x3 privInv3x3(const QMatrix3x3 in)
    {
        float p[] = { (in(1,1)*in(2,2) - in(1,2)*in(2,1)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)), -(in(0,1)*in(2,2) - in(0,2)*in(2,1)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)),  (in(0,1)*in(1,2) - in(0,2)*in(1,1)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)),
            -(in(1,0)*in(2,2) - in(1,2)*in(2,0)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)),  (in(0,0)*in(2,2) - in(0,2)*in(2,0)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)), -(in(0,0)*in(1,2) - in(0,2)*in(1,0)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)),
            (in(1,0)*in(2,1) - in(1,1)*in(2,0)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)), -(in(0,0)*in(2,1) - in(0,1)*in(2,0)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0)),  (in(0,0)*in(1,1) - in(0,1)*in(1,0)) / (in(0,0)*in(1,1)*in(2,2) - in(0,0)*in(1,2)*in(2,1) - in(0,1)*in(1,0)*in(2,2) + in(0,1)*in(1,2)*in(2,0) + in(0,2)*in(1,0)*in(2,1) - in(0,2)*in(1,1)*in(2,0))
        };
        QMatrix3x3 out(p);
        return out;
    }
public:

    Zsteam() :m_izBuff(QOpenGLBuffer::VertexBuffer), m_xyBuff(QOpenGLBuffer::VertexBuffer), m_iBuff(QOpenGLBuffer::IndexBuffer), m_glInitialized(false), m_meshShader() {}

    int load(const QString& str)
    {
        std::ifstream fileStream(str.toStdString(), std::ios::binary);
        char buffer[1024];
        fileStream.read(buffer, 1024);

        if (memcmp(buffer, ".3ds", 4) != 0)
            return -1;
        float* buffer_f = reinterpret_cast<float*>(buffer);
      

        //uint32_t flags = *reinterpret_cast<uint32_t*>(buffer_f + 1);
        m_fd.z2mm = buffer_f[2];
        
        std::array<uint16_t, 2> wh = *reinterpret_cast<std::array<uint16_t, 2>*>(buffer_f + 3);
        m_fd.width = wh[0];
        m_fd.height = wh[1];
        QMatrix3x3 k;
        std::copy(buffer_f + 4, buffer_f + 13, k.data());
        m_fd.kinv = privInv3x3(k);
        m_fd.indices = privTriangulate(wh[0], wh[1]);
      
        m_zdata.resize(wh[0] * wh[1]);
        m_fd.xy.resize(wh[0] * wh[1]);
        for (int i = 0; i != wh[0] * wh[1]; ++i)
        {
            m_fd.xy[i][0] = i%wh[0];
            m_fd.xy[i][1] = i/wh[0];
        }

        fileStream.read(reinterpret_cast<char*>(&m_zdata[0]), wh[0] * wh[1] * 4);
        initGL();
        return 0;

    }
    bool isValid() const
    {
        return !m_zdata.empty();
    }

    void initGL()
    {
        if (m_glInitialized)
            return;
        initializeOpenGLFunctions();
        m_izBuff.create();
        m_iBuff.create();
        m_xyBuff.create();

        m_izBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        m_xyBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);



        //create unindex vertex data;
        
        m_zdata = { 20,20,20,20 };
        std::vector<std::array<float, 2>> xy = { {0,0},{639,0},{0,479},{639,479} };
        //std::vector<std::array<float, 2>> xy = { { 0,0 },{ 1,0 },{ 0,1},{ 1,1} };
        
        unsigned int iFlat[] = { 0,1,2,1,2,3 };


        m_izBuff.bind();
        m_izBuff.allocate(m_zdata.data(), int(m_zdata.size() * sizeof(float) ));
        m_izBuff.release();

        m_xyBuff.bind();
        m_xyBuff.allocate(xy.data(), int(xy.size() * 2*sizeof(float)));
        m_xyBuff.release();


        m_iBuff.bind();
        int n = int(sizeof(iFlat) );
        m_iBuff.allocate(iFlat,n);

        m_iBuff.release();



        m_glInitialized = true;
        initShader("zmeshv", "zmeshf");

    }

    bool initShader(const QString& vshader, const QString& fshader)
    {
        // Compile vertex shader
        if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/" + vshader + ".glsl"))
        {
            QString err = m_meshShader.log();
            return false;
        }

        // Compile fragment shader
        if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/" + fshader + ".glsl"))
            QString err = m_meshShader.log();
            return false;

        // Link shader pipeline
        if (!m_meshShader.link())
            return false;
        return true;


    }

    void draw(const QMatrix4x4& mvp, int textureType)
    {
    

        
        


        m_meshShader.bind();
        m_meshShader.setUniformValue("u_mvp", mvp);


        m_meshShader.setUniformValue("u_kinv", m_fd.kinv);

        //QMatrix3x3 rotMat;
        //rotMat.setToIdentity();



        m_iBuff.bind();

        m_izBuff.bind();
        m_izBuff.write(0, m_zdata.data(), int(m_zdata.size() * sizeof(float) * 3));
        const GLuint vp = m_meshShader.attributeLocation("a_z");
        m_meshShader.enableAttributeArray(vp);
        m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 1, sizeof(float) * 1);
        

        const GLuint vc = m_meshShader.attributeLocation("a_rgb");
        m_meshShader.enableAttributeArray(vc);
        m_meshShader.setAttributeBuffer(vc, GL_FLOAT, 0 * sizeof(float), 3, sizeof(float) * 3);


        m_izBuff.release();


        m_xyBuff.bind();
        const GLuint vu = m_meshShader.attributeLocation("a_u");
        m_meshShader.enableAttributeArray(vu);
        m_meshShader.setAttributeBuffer(vu, GL_FLOAT, 0 * sizeof(float), 1, sizeof(float) * 2);
        const GLuint vv = m_meshShader.attributeLocation("a_v");
        m_meshShader.enableAttributeArray(vv);
        m_meshShader.setAttributeBuffer(vv, GL_FLOAT, 1 * sizeof(float), 1, sizeof(float) * 2);
        m_xyBuff.release();



        
        

        
        

        
        

        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        int n = m_iBuff.size() / sizeof(uint32_t);
        glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, NULL);
        //

        
        m_meshShader.release();
        m_iBuff.release();

        //int vp = m_meshShader.attributeLocation("a_xyz");
        //m_meshShader.enableAttributeArray(vp);
        //m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(float) * 3);


        //int vc = m_meshShader.attributeLocation("a_rgb");
        //m_meshShader.enableAttributeArray(vc);
        //m_meshShader.setAttributeBuffer(vc, GL_FLOAT, 0 * sizeof(float), 3, sizeof(float) * 3);




        //m_meshShader.setUniformValue("u_txt", 0);


        ////glVertexAttribPointer(vc, 3, GL_UNSIGNED_INT8_NV, false, 3 * sizeof(uint8_t), NULL);
        //glDrawElements(GL_TRIANGLES, m_iBuff.size() / sizeof(uint32_t), GL_UNSIGNED_INT, NULL);

        //m_izBuff.release();
        //m_iBuff.release();

        //// Clean up state machine
        m_meshShader.disableAttributeArray(vp);
        m_meshShader.disableAttributeArray(vc);
        m_meshShader.disableAttributeArray(vu);
        m_meshShader.disableAttributeArray(vv);



    }


};
