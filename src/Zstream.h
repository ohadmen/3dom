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
#include <qtGui/qopenglext.h>
//c:\Qt\Qt5.7.1\5.7\msvc2015_64\include\QtGui\qopenglext"
#include "Array2d.h"


class Zsteam : protected QOpenGLFunctions
{

    typedef std::vector<unsigned int> IndArr;
    typedef std::vector<std::array<std::uint16_t, 2>> Zdata;
    //--------------INTERNAL TYPE DECL---------------
    struct FrameData
    {
        IndArr indices;
        std::vector<std::array<float, 2>> xy;
        QMatrix3x3 kinv;
        float z2mm;
        uint16_t width;
        uint16_t height;
        uint32_t flags;


    };
    //--------------DATA MEMBERS---------------
    FrameData m_fd;
    std::vector<float>         m_zdata;
    std::vector<std::array<float, 3>>         m_cdata;
    QOpenGLBuffer m_zBuff; //z
    QOpenGLBuffer m_uvBuff; //uv
    QOpenGLBuffer m_cBuff; //uv
    QOpenGLBuffer m_iBuff; //indices

    bool m_glInitialized;
    QOpenGLShaderProgram m_meshShader;

    static  IndArr privTriangulate(uint16_t w, uint16_t h)
    {
        uint32_t key = (uint32_t(w) << 16) + h;
        static std::map<uint32_t, IndArr> map;

        if (map.find(key) == map.end())
        {
            IndArr a((w + 1)*(h - 1) * 2);
            auto sub2ind = [&w](int xx, int yy) {return xx + yy*w; };
            for (int y = 0; y != h - 1; ++y)
            {
                auto arow = &a[(w + 1) * 2 * y];
                for (int x = 0; x != w; ++x)
                {
                    arow[2 * x + 0] = sub2ind(x, y);
                    arow[2 * x + 1] = sub2ind(x, y + 1);
                }
                arow[2 * w] = sub2ind(w - 1, y + 1);
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

    Zsteam() :m_zBuff(QOpenGLBuffer::VertexBuffer), m_uvBuff(QOpenGLBuffer::VertexBuffer), m_cBuff(QOpenGLBuffer::VertexBuffer), m_iBuff(QOpenGLBuffer::IndexBuffer), m_glInitialized(false), m_meshShader() {}

    int load(const QString& str)
    {
        std::ifstream fileStream(str.toStdString(), std::ios::binary);
        char buffer[1024];
        fileStream.read(buffer, 1024);

        if (memcmp(buffer, ".3ds", 4) != 0)
            return -1;
        float* buffer_f = reinterpret_cast<float*>(buffer);


        m_fd.flags = *reinterpret_cast<uint32_t*>(buffer_f + 1);
        m_fd.z2mm = buffer_f[2];

        std::array<uint16_t, 2> wh = *reinterpret_cast<std::array<uint16_t, 2>*>(buffer_f + 3);


        m_fd.width = wh[0];
        m_fd.height = wh[1];
        QMatrix3x3 k;
        std::copy(buffer_f + 4, buffer_f + 13, k.data());
        

       
        
        m_fd.kinv = privInv3x3(k);



        m_fd.indices = privTriangulate(wh[0], wh[1]);

        int n = wh[0] * wh[1];
        m_zdata.resize(n);
        m_fd.xy.resize(n);
        m_cdata.resize(n);
        for (int i = 0; i != n; ++i)
        {
            m_fd.xy[i][0] = (i%wh[0])*639/(wh[0]-1);
            m_fd.xy[i][1] = (i / wh[0])*479/(wh[1]-1);
        }


        initGL();


        std::vector<unsigned short> dataBuff(getWidth()*getHeight()*2);
        fileStream.read(reinterpret_cast<char*>(&dataBuff[0]), wh[0] * wh[1] * 4);
        setBuffer(dataBuff.data());

        return 0;

    }
    int getWidth() const { return m_fd.width; }
    int getHeight() const { return m_fd.height; }
    template<class T>
    void setBuffer(const T* data)
    {

        for (int i = 0; i != m_zdata.size(); ++i)
        {
            m_zdata[i] = data[i * 2] / m_fd.z2mm;
            
        }

        if ((m_fd.flags&uint32_t(3)) == 1)
            //8bit to rgb
        {
            auto frac = [](float f)->float {return f - floor(f); };
            for (int i = 0; i != m_zdata.size(); ++i)
            {
                float c = float(data[i * 2 + 1]) / 255;
                m_cdata[i] = { c,c,c };
        }
        }

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
        m_zBuff.create();
        m_iBuff.create();
        m_uvBuff.create();
        m_cBuff.create();

        m_zBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        m_cBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        m_uvBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);



        //m_zdata = { 0,1000,0,0,0,0,0,0,0,0 };
        //m_fd.xy = { { 0,0 },{100,0}, { 639,0 },{0,100},{100,100}, { 639,100 },{0,479},{100,479},{ 639,479 } };
        //m_cdata = { {0,0,0},{0,0,1},{0,1,0},{0,1,0},{0,1,1},{1,0,0},{1,0,1},{1,1,0},{1,1,1},{1,1,1} };
        //m_fd.indices = { 0,3,1,4,2,5,5,3,3,6,4,7,5,8 };


 


        m_zBuff.bind();
        m_zBuff.allocate(m_zdata.data(), int(m_zdata.size() * sizeof(float)));
        m_zBuff.release();

        m_uvBuff.bind();
        m_uvBuff.allocate(m_fd.xy.data(), int(m_fd.xy.size() * 2 * sizeof(float)));
        m_uvBuff.release();

        m_cBuff.bind();
        m_cBuff.allocate(m_cdata.data(), int(m_cdata.size() * 3 * sizeof(float)));
        m_cBuff.release();


        m_iBuff.bind();
        m_iBuff.allocate(m_fd.indices.data(),  int(m_fd.indices.size()  * sizeof(unsigned int)));
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
    QVector3D getCenter() const
    {
        const int w = m_fd.width;
        const int h = m_fd.height;
        auto sub2ind = [&w](int xx, int yy) {return xx + yy*w; };
        //float zc = m_zdata[sub2ind(w / h, h / 2)];
        float zc = m_zdata[0];
        return QVector3D(0,0,zc);


    }
    float getContainmentRadius() const
    {
        float x =  m_fd.width*m_fd.kinv(0,0) * (*std::max_element(m_zdata.begin(), m_zdata.end()));
        return x;
    }

    void draw(const QMatrix4x4& mvp, int textureType)
    {

        
        m_meshShader.bind();
        m_meshShader.setUniformValue("u_mvp", mvp);


        m_meshShader.setUniformValue("u_kinv", m_fd.kinv);

        //QMatrix3x3 rotMat;
        //rotMat.setToIdentity();



        m_iBuff.bind();

        m_zBuff.bind();
        m_zBuff.write(0, m_zdata.data(), int(m_zdata.size() * sizeof(float) ));
        const GLuint vp = m_meshShader.attributeLocation("a_z");
        m_meshShader.enableAttributeArray(vp);
        m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 1, sizeof(float) * 1);
        m_zBuff.release();


        m_cBuff.bind();
        m_cBuff.write(0, m_cdata.data(), int(m_cdata.size() * sizeof(float)*3));
        const GLuint vc = m_meshShader.attributeLocation("a_rgb");
        m_meshShader.enableAttributeArray(vc);
        m_meshShader.setAttributeBuffer(vc, GL_FLOAT, 0 * sizeof(float), 3, sizeof(float) * 3);
        m_cBuff.release();

        m_uvBuff.bind();
        const GLuint vu = m_meshShader.attributeLocation("a_uv");
        m_meshShader.enableAttributeArray(vu);
        m_meshShader.setAttributeBuffer(vu, GL_FLOAT, 0 * sizeof(float), 2, sizeof(float) * 2);

        m_uvBuff.release();












        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        int n = m_iBuff.size() / sizeof(uint32_t);
        glDrawElements(GL_TRIANGLE_STRIP, n, GL_UNSIGNED_INT, NULL);
        //


        m_meshShader.release();
        m_iBuff.release();


        m_meshShader.disableAttributeArray(vp);
        m_meshShader.disableAttributeArray(vc);
        m_meshShader.disableAttributeArray(vu);




    }


};
