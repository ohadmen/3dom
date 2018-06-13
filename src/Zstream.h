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
        float k[9];
        float z2mm;
        uint16_t width;
        uint16_t height;
        
    };
    //--------------DATA MEMBERS---------------
    FrameData m_fd;
    Zdata         m_zdata;
    QOpenGLBuffer m_vBuff; //vertices 3xn
    QOpenGLBuffer m_iBuff; //indices 3xm

    bool m_glInitialized;
    QOpenGLShaderProgram m_meshShader;

    static  IndArr privTriangulate(uint16_t w, uint16_t h)
    {
        uint32_t key = (uint32_t(w) << 16) + h;
        static std::map<uint32_t, IndArr> map;

        if (map.at(key).empty())
        {
            IndArr a((w - 1)*(h - 1) * 2*3);
            auto sub2ind = [&w](int xx, int yy) {return xx + yy*w; };
            for (int y = 0; y != h - 1; ++y)
            {
                for (int x = 0; x != w - 1; ++x)
                {
                    int i = x + y*(w - 1);
                    a[(2 * i + 0) * 3 + 0] = a[(2 * i + 1) * 3 + 0] =sub2ind(x, y);
                    a[(2 * i + 0) * 3 + 1] = a[(2 * i + 1) * 3 + 2] = sub2ind(x + 1, y + 1);
                    a[(2 * i + 0) * 3 + 2] = sub2ind(x + 1, y);
                    //a[(2 * i + 1) * 3 + 0] = sub2ind(x, y);
                    a[(2 * i + 1) * 3 + 1] = sub2ind(x, y + 1);
                    //a[(2 * i + 1) * 3 + 2] = sub2ind(x + 1, y + 1);
                }
            }
            map.insert(std::make_pair(key, a));
        }
        return map.at(key);
    }

public:
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
        std::copy(buffer_f + 4, buffer_f + 13, m_fd.k);
        m_fd.indices = privTriangulate(wh[0], wh[1]);
      
        m_zdata.resize(wh[0] * wh[1]);
        fileStream.read(reinterpret_cast<char*>(&m_zdata[0]), wh[0] * wh[1] * 4);

        return 0;

    }


    void initGL()
    {
        if (m_glInitialized)
            return;
        initializeOpenGLFunctions();
        m_vBuff.create();
        m_iBuff.create();


        m_vBuff.setUsagePattern(QOpenGLBuffer::StreamDraw);
        m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);


       

        m_vBuff.bind();
        m_vBuff.allocate(m_zdata.data(), int(m_zdata.size() * sizeof(uint16_t)));
        m_vBuff.release();

        m_iBuff.bind();
        m_iBuff.allocate(m_fd.indices.data(), int(m_fd.indices.size() * sizeof(unsigned int)));
        m_iBuff.release();



        m_glInitialized = true;
        initShader("meshv", "meshf");

    }

    bool initShader(const QString& vshader, const QString& fshader)
    {
        // Compile vertex shader
        if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/" + vshader + ".glsl"))
            return false;

        // Compile fragment shader
        if (!m_meshShader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/" + fshader + ".glsl"))
            return false;

        // Link shader pipeline
        if (!m_meshShader.link())
            return false;
        return true;

    }

    void draw(const QMatrix4x4& mvp, int textureType)
    {
        m_meshShader.bind();
        m_meshShader.setUniformValue("mvp_matrix", mvp);
        //QMatrix3x3 rotMat;
        //rotMat.setToIdentity();







        m_vBuff.bind();
        m_vBuff.write(0, m_vFlat.data(), int(m_vFlat.size() * sizeof(float) * 9));


        m_iBuff.bind();



        int vp = m_meshShader.attributeLocation("a_xyz");
        m_meshShader.enableAttributeArray(vp);
        m_meshShader.setAttributeBuffer(vp, GL_FLOAT, 0 * sizeof(float), 3, sizeof(float) * 9);


        int vc = m_meshShader.attributeLocation("a_rgb");
        m_meshShader.enableAttributeArray(vc);
        m_meshShader.setAttributeBuffer(vc, GL_FLOAT, 3 * sizeof(float), 3, sizeof(float) * 9);

        int vn = m_meshShader.attributeLocation("a_nrml");
        m_meshShader.enableAttributeArray(vn);
        m_meshShader.setAttributeBuffer(vn, GL_FLOAT, 6 * sizeof(float), 3, sizeof(float) * 9);


        m_meshShader.setUniformValue("u_txt", textureType);


        //glVertexAttribPointer(vc, 3, GL_UNSIGNED_INT8_NV, false, 3 * sizeof(uint8_t), NULL);
        glDrawElements(GL_TRIANGLES, m_iBuff.size() / sizeof(uint32_t), GL_UNSIGNED_INT, NULL);

        m_vBuff.release();
        m_iBuff.release();

        // Clean up state machine
        m_meshShader.disableAttributeArray(vp);
        m_meshShader.disableAttributeArray(vc);




    }


};
