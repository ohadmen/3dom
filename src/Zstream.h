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


class Zstream : protected QOpenGLFunctions
{

    typedef std::vector<unsigned int> IndArr;
    typedef std::array<float, 3> Col;

    //--------------INTERNAL TYPE DECL---------------
    struct FrameData
    {
        IndArr indices;
        std::vector<std::array<float, 2>> xy;
        QMatrix3x3 kinv;
        QMatrix3x3 k;
        float z2mm;
        uint16_t width;
        uint16_t height;
        uint32_t flags;
        float fps;


    };
    //--------------DATA MEMBERS---------------
    FrameData m_fd;
    std::vector<float>         m_zdata;
    std::vector<Col>         m_cdata;
    QOpenGLBuffer m_zBuff; //z
    QOpenGLBuffer m_uvBuff; //uv
    QOpenGLBuffer m_cBuff; //uv
    QOpenGLBuffer m_iBuff; //indices

    bool m_glInitialized;
    QOpenGLShaderProgram m_meshShader;
    QWidget* m_drawingWidget;

    mutable std::mutex m_cmtx;
    mutable std::mutex m_zmtx;


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

    //singleton, private constructor
    Zstream() :m_drawingWidget(nullptr), m_zBuff(QOpenGLBuffer::VertexBuffer), m_uvBuff(QOpenGLBuffer::VertexBuffer), m_cBuff(QOpenGLBuffer::VertexBuffer), m_iBuff(QOpenGLBuffer::IndexBuffer), m_glInitialized(false), m_meshShader()
    {
        initializeOpenGLFunctions();
        m_zBuff.create();
        m_iBuff.create();
        m_uvBuff.create();
        m_cBuff.create();

        m_zBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        m_cBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        m_uvBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        initShader("zmeshv", "zmeshf");
    }

public:

    void setDrawingWidget(QWidget* w)
    {
        m_drawingWidget = w;
    }

    static Zstream& i()
    {
        static Zstream    instance; // Guaranteed to be destroyed.

        return instance;
    }



    void set(const std::array<uint16_t, 2>& wh, float* k, float z2mm, float fps, uint32_t flags)
    {
        setStreamSize(wh);
        setK(k);
        setZ2mm(z2mm);
        setFlags(flags);
        setFPS(fps);
        initGL();
    }
    void setFlags(uint32_t flags) { m_fd.flags = flags; }
    uint32_t getFlags()const { return m_fd.flags; }
    void setZ2mm(float z2mm)
    {
        m_fd.z2mm = z2mm;
    }
    float getZ2mm()const { return m_fd.z2mm; }
    void setK(float *v)
    {
        m_fd.k = QMatrix3x3(v);
        m_fd.kinv = privInv3x3(m_fd.k.transposed());
    }
    void setFPS(float fps) { m_fd.fps = fps; }
    float getFPS()const { return m_fd.fps; }
    const QMatrix3x3& getK() const { return m_fd.k; }
    void setStreamSize(const std::array<uint16_t, 2>& wh)
    {

        m_fd.width = wh[0];
        m_fd.height = wh[1];
        m_fd.indices = privTriangulate(wh[0], wh[1]);

        int n = wh[0] * wh[1];
        m_zdata.resize(n);
        m_fd.xy.resize(n);
        m_cdata.resize(n);
        for (int i = 0; i != n; ++i)
        {
            m_fd.xy[i][0] = (i % wh[0]);
            m_fd.xy[i][1] = (i / wh[0]);
        }

    }

    int getWidth() const { return m_fd.width; }
    int getHeight() const { return m_fd.height; }

    void  copyZdata(uint16_t* dst)const
    {
        std::lock_guard<std::mutex> grd(m_zmtx);
        const float& z2mm = m_fd.z2mm;
        std::transform(m_zdata.begin(), m_zdata.end(), dst, [&](float v)->uint16_t
        {
            return uint16_t(v*z2mm); }
        );
    }

    void  copyCdata(uint16_t* dst)const
    {
        std::lock_guard<std::mutex> grd(m_cmtx);
        const float& z2mm = m_fd.z2mm;
        if (m_fd.flags == 1)
            std::transform(m_cdata.begin(), m_cdata.end(), dst, [&](const Col& v)->uint16_t
        {
            return uint16_t(v[0] * 255); }
        );
    }


    template<class T, class K>
    void setBuffer(const T* zdata, const K* colData)
    {
        {
            std::lock_guard<std::mutex> grd(m_zmtx);
            //float a = 0.2;
            for (int i = 0; i != m_zdata.size(); ++i)
            {
                if (zdata[i] == 0)
                    m_zdata[i] = std::numeric_limits<float>::quiet_NaN();
                else
                {
                    m_zdata[i] = float(zdata[i]) / m_fd.z2mm;
                    /* if (isnan(m_zdata[i]))
                     {
                         m_zdata[i] = float(zdata[i]) / m_fd.z2mm;
                     }
                     else
                     {
                         m_zdata[i] = (1 - a)*m_zdata[i] + a*float(zdata[i]) / m_fd.z2mm;
                     }*/

                }

            }
        }
        {
            std::lock_guard<std::mutex> grd(m_cmtx);
            if ((m_fd.flags&uint32_t(3)) == 1)
                //8bit to rgb
            {
                for (int i = 0; i != m_zdata.size(); ++i)
                {
                    float c = float(colData[i]) / 255;
                    m_cdata[i] = { c,c,c };
                }
            }
            if (m_drawingWidget)
                m_drawingWidget->update();
        }

    }

    bool isValid() const
    {
        return m_glInitialized;
    }


    void initGL()
    {
        if (m_glInitialized)
            return;
        initializeOpenGLFunctions();
        //m_zBuff.create();
        //m_iBuff.create();
        //m_uvBuff.create();
        //m_cBuff.create();
        //
        //m_zBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        //m_cBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        //m_uvBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        //m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);



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
        m_iBuff.allocate(m_fd.indices.data(), int(m_fd.indices.size() * sizeof(unsigned int)));
        m_iBuff.release();





        m_glInitialized = true;

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
        return QVector3D(0, 0, zc);


    }
    float getContainmentRadius() const
    {
        static const float z0 = 1000;//field of view to catch
        float x = m_fd.width*m_fd.kinv(0, 0) * z0;
        return x;
    }

    void draw(const QMatrix4x4& mvp, int textureType)
    {

        static const GLfloat colormap[] = { 0, 0, 0.515625, 0, 0, 0.53125, 0, 0, 0.546875, 0, 0, 0.5625, 0, 0, 0.578125, 0, 0, 0.59375, 0, 0, 0.609375, 0, 0, 0.625, 0, 0, 0.640625, 0, 0, 0.65625, 0, 0, 0.671875, 0, 0, 0.6875, 0, 0, 0.703125, 0, 0, 0.71875, 0, 0, 0.734375, 0, 0, 0.75, 0, 0, 0.765625, 0, 0, 0.78125, 0, 0, 0.796875, 0, 0, 0.8125, 0, 0, 0.828125, 0, 0, 0.84375, 0, 0, 0.859375, 0, 0, 0.875, 0, 0, 0.890625, 0, 0, 0.90625, 0, 0, 0.921875, 0, 0, 0.9375, 0, 0, 0.953125, 0, 0, 0.96875, 0, 0, 0.984375, 0, 0, 1, 0, 0.015625, 1, 0, 0.03125, 1, 0, 0.046875, 1, 0, 0.0625, 1, 0, 0.078125, 1, 0, 0.09375, 1, 0, 0.109375, 1, 0, 0.125, 1, 0, 0.140625, 1, 0, 0.15625, 1, 0, 0.171875, 1, 0, 0.1875, 1, 0, 0.203125, 1, 0, 0.21875, 1, 0, 0.234375, 1, 0, 0.25, 1, 0, 0.265625, 1, 0, 0.28125, 1, 0, 0.296875, 1, 0, 0.3125, 1, 0, 0.328125, 1, 0, 0.34375, 1, 0, 0.359375, 1, 0, 0.375, 1, 0, 0.390625, 1, 0, 0.40625, 1, 0, 0.421875, 1, 0, 0.4375, 1, 0, 0.453125, 1, 0, 0.46875, 1, 0, 0.484375, 1, 0, 0.5, 1, 0, 0.515625, 1, 0, 0.53125, 1, 0, 0.546875, 1, 0, 0.5625, 1, 0, 0.578125, 1, 0, 0.59375, 1, 0, 0.609375, 1, 0, 0.625, 1, 0, 0.640625, 1, 0, 0.65625, 1, 0, 0.671875, 1, 0, 0.6875, 1, 0, 0.703125, 1, 0, 0.71875, 1, 0, 0.734375, 1, 0, 0.75, 1, 0, 0.765625, 1, 0, 0.78125, 1, 0, 0.796875, 1, 0, 0.8125, 1, 0, 0.828125, 1, 0, 0.84375, 1, 0, 0.859375, 1, 0, 0.875, 1, 0, 0.890625, 1, 0, 0.90625, 1, 0, 0.921875, 1, 0, 0.9375, 1, 0, 0.953125, 1, 0, 0.96875, 1, 0, 0.984375, 1, 0, 1, 1, 0.015625, 1, 0.984375, 0.03125, 1, 0.96875, 0.046875, 1, 0.953125, 0.0625, 1, 0.9375, 0.078125, 1, 0.921875, 0.09375, 1, 0.90625, 0.109375, 1, 0.890625, 0.125, 1, 0.875, 0.140625, 1, 0.859375, 0.15625, 1, 0.84375, 0.171875, 1, 0.828125, 0.1875, 1, 0.8125, 0.203125, 1, 0.796875, 0.21875, 1, 0.78125, 0.234375, 1, 0.765625, 0.25, 1, 0.75, 0.265625, 1, 0.734375, 0.28125, 1, 0.71875, 0.296875, 1, 0.703125, 0.3125, 1, 0.6875, 0.328125, 1, 0.671875, 0.34375, 1, 0.65625, 0.359375, 1, 0.640625, 0.375, 1, 0.625, 0.390625, 1, 0.609375, 0.40625, 1, 0.59375, 0.421875, 1, 0.578125, 0.4375, 1, 0.5625, 0.453125, 1, 0.546875, 0.46875, 1, 0.53125, 0.484375, 1, 0.515625, 0.5, 1, 0.5, 0.515625, 1, 0.484375, 0.53125, 1, 0.46875, 0.546875, 1, 0.453125, 0.5625, 1, 0.4375, 0.578125, 1, 0.421875, 0.59375, 1, 0.40625, 0.609375, 1, 0.390625, 0.625, 1, 0.375, 0.640625, 1, 0.359375, 0.65625, 1, 0.34375, 0.671875, 1, 0.328125, 0.6875, 1, 0.3125, 0.703125, 1, 0.296875, 0.71875, 1, 0.28125, 0.734375, 1, 0.265625, 0.75, 1, 0.25, 0.765625, 1, 0.234375, 0.78125, 1, 0.21875, 0.796875, 1, 0.203125, 0.8125, 1, 0.1875, 0.828125, 1, 0.171875, 0.84375, 1, 0.15625, 0.859375, 1, 0.140625, 0.875, 1, 0.125, 0.890625, 1, 0.109375, 0.90625, 1, 0.09375, 0.921875, 1, 0.078125, 0.9375, 1, 0.0625, 0.953125, 1, 0.046875, 0.96875, 1, 0.03125, 0.984375, 1, 0.015625, 1, 1, 0, 1, 0.984375, 0, 1, 0.96875, 0, 1, 0.953125, 0, 1, 0.9375, 0, 1, 0.921875, 0, 1, 0.90625, 0, 1, 0.890625, 0, 1, 0.875, 0, 1, 0.859375, 0, 1, 0.84375, 0, 1, 0.828125, 0, 1, 0.8125, 0, 1, 0.796875, 0, 1, 0.78125, 0, 1, 0.765625, 0, 1, 0.75, 0, 1, 0.734375, 0, 1, 0.71875, 0, 1, 0.703125, 0, 1, 0.6875, 0, 1, 0.671875, 0, 1, 0.65625, 0, 1, 0.640625, 0, 1, 0.625, 0, 1, 0.609375, 0, 1, 0.59375, 0, 1, 0.578125, 0, 1, 0.5625, 0, 1, 0.546875, 0, 1, 0.53125, 0, 1, 0.515625, 0, 1, 0.5, 0, 1, 0.484375, 0, 1, 0.46875, 0, 1, 0.453125, 0, 1, 0.4375, 0, 1, 0.421875, 0, 1, 0.40625, 0, 1, 0.390625, 0, 1, 0.375, 0, 1, 0.359375, 0, 1, 0.34375, 0, 1, 0.328125, 0, 1, 0.3125, 0, 1, 0.296875, 0, 1, 0.28125, 0, 1, 0.265625, 0, 1, 0.25, 0, 1, 0.234375, 0, 1, 0.21875, 0, 1, 0.203125, 0, 1, 0.1875, 0, 1, 0.171875, 0, 1, 0.15625, 0, 1, 0.140625, 0, 1, 0.125, 0, 1, 0.109375, 0, 1, 0.09375, 0, 1, 0.078125, 0, 1, 0.0625, 0, 1, 0.046875, 0, 1, 0.03125, 0, 1, 0.015625, 0, 1, 0, 0, 0.984375, 0, 0, 0.96875, 0, 0, 0.953125, 0, 0, 0.9375, 0, 0, 0.921875, 0, 0, 0.90625, 0, 0, 0.890625, 0, 0, 0.875, 0, 0, 0.859375, 0, 0, 0.84375, 0, 0, 0.828125, 0, 0, 0.8125, 0, 0, 0.796875, 0, 0, 0.78125, 0, 0, 0.765625, 0, 0, 0.75, 0, 0, 0.734375, 0, 0, 0.71875, 0, 0, 0.703125, 0, 0, 0.6875, 0, 0, 0.671875, 0, 0, 0.65625, 0, 0, 0.640625, 0, 0, 0.625, 0, 0, 0.609375, 0, 0, 0.59375, 0, 0, 0.578125, 0, 0, 0.5625, 0, 0, 0.546875, 0, 0, 0.53125, 0, 0, 0.515625, 0, 0, 0.5, 0, 0 };


        m_meshShader.bind();
        m_meshShader.setUniformValue("u_mvp", mvp);

        m_meshShader.setUniformValue("u_txt", textureType);
        m_meshShader.setUniformValue("u_kinv", m_fd.kinv);
        m_meshShader.setUniformValueArray("u_colmap", colormap, 256, 3);

        //QMatrix3x3 rotMat;
        //rotMat.setToIdentity();



        m_iBuff.bind();

        m_zBuff.bind();
        m_zBuff.write(0, m_zdata.data(), int(m_zdata.size() * sizeof(float)));
        const GLuint vp = m_meshShader.attributeLocation("a_z");
        m_meshShader.enableAttributeArray(vp);
        m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 1, sizeof(float) * 1);
        m_zBuff.release();


        m_cBuff.bind();
        m_cBuff.write(0, m_cdata.data(), int(m_cdata.size() * sizeof(float) * 3));
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

    /*   bool closest2ray(const QLine3D& line, QVector3D* ptP) const
       {
           const QMatrix3x3& m = m_fd.k;

           const float* f = m.data();

           auto project = [&f](const QVector3D& v)->QVector2D
           {
               QVector3D u(
                   f[0] * v.x + f[1] * v.y + f[2] * v.z,
                   f[3] * v.x + f[4] * v.y + f[5] * v.z,
                   f[6] * v.x + f[7] * v.y + f[8] * v.z
               );
               return QVector2D(u.x / u.z, u.y / u.z);

           }
           ;
           QVector2D uv1 = project(line.p1());
           QVector2D uv2 = project(line.p2());



       }*/

       /*  static std::vector<QVector2D> sprivBresIndex(const QVector2D& s, QVector2D,const QVector2D& d)
         {
             QVector2D n = d - s;
             auto p = [&](float t) {return s + n*t; };
             auto floorV = [](const QVector2D& v) {return QVector2D(floor(v.x()), floor(v.y()));  };
             auto ceilV = [](const QVector2D& v) {return QVector2D(ceil(v.x), ceil(v.y));  };
             auto arr4 = [](const QVector2D& v1, const QVector2D& v2) {std::array<float, 4> a = { v1.x,v1.y,v2.x,v2.y }; return a; };
             float t = 0;
             std::vector<QVector2D> pt(1, s);
             float eps = std::numeric_limits<float>::epsilon()*1e-3;
             while (t < 1)
             {
                 auto a = arr4((floorV(p(t+eps)) - s) / (d - s), (ceilV(p(t+eps)) - s) / (d - s));
                 std::for_each(a.begin(), a.end(), [&t](float& v) {v = v <= t ? 1 : v; });
                 float tnext = std::min(1.0f, *std::min_element(a.begin(), a.end()));
                 t = tnext;
                 pt.push_back(p(t));

             }
             return pt;

         }
     */
};
