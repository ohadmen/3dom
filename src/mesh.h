#ifndef MESH_H
#define MESH_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <QVector3D>
#include <algorithm>
#include <vector>
#include <map>

#include <array>
#include "Shader.h"
#include "Q3d/QLine3D.h"

class Mesh : protected QOpenGLFunctions
{
public:
    struct VertData
    {
        float x;
        float y;
        float z;
        float r;
        float g;
        float b;
        VertData() : x(0), y(0), z(0),r(0),g(0),b(0) {}
        VertData(float x, float y, float z) : x(x), y(y), z(z),r(0),g(0),b(0) {}
        operator QVector3D() const { return QVector3D(x, y, z); }
        bool operator!=(const VertData& rhs) const
        {
            return x != rhs.x || y != rhs.y || z != rhs.z;
        }
        bool operator<(const VertData& rhs) const
        {
            if (x != rhs.x)    return x < rhs.x;
            else if (y != rhs.y)    return y < rhs.y;
            else if (z != rhs.z)    return z < rhs.z;
            else                    return false;
        }
    };
private:

    
    std::vector<VertData> m_vertices;
    std::vector<GLuint> m_indices;//concatenated tri indices
    QVector3D m_objCenter;
    float     m_objcontainerRadius;
    QOpenGLBuffer m_vBuff;
    QOpenGLBuffer m_iBuff;
    bool m_glInitialized;
    Shader m_meshShader;
public:
    Mesh():m_vBuff(QOpenGLBuffer::VertexBuffer), m_iBuff(QOpenGLBuffer::IndexBuffer), m_glInitialized(false), m_meshShader(){
        
    }
    ~Mesh()
    {
        if (m_vBuff.isCreated())
            m_vBuff.destroy();
        if (m_iBuff.isCreated())
            m_iBuff.destroy();
    }
    void set(const std::vector<VertData>&& v, const std::vector<GLuint>&& i)
    {

        m_vertices = v;
        m_indices = i;
        m_objCenter = std::accumulate(m_vertices.begin(), m_vertices.end(), QVector3D(), [](const QVector3D& s,const VertData& v) {return s + v; }) / float(v.size());
        auto p = std::max_element(m_vertices.begin(), m_vertices.end(), [&](const VertData& a, const VertData&b) {return (a - m_objCenter).length() < (b - m_objCenter).length(); });
        m_objcontainerRadius = (*p - m_objCenter).length();
    }
    QVector3D getCenter() const {return m_objCenter; }
    float getContainmentRadius() const {return m_objcontainerRadius; }
    void initGL()
    {
        if (m_glInitialized)
            return;
        initializeOpenGLFunctions();
        m_vBuff.create();
        m_iBuff.create();

        m_vBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);
        m_iBuff.setUsagePattern(QOpenGLBuffer::StaticDraw);


        m_vBuff.bind();
        m_vBuff.allocate(m_vertices.data(), int(m_vertices.size() * sizeof(VertData)));
        m_vBuff.release();

        m_iBuff.bind();
        m_iBuff.allocate(m_indices.data(), int(m_indices.size() * sizeof(uint32_t)));
        m_iBuff.release();
        m_glInitialized = true;
        m_meshShader.init("meshv", "meshf");

    }

    bool closest2ray(const QLine3D& line, QVector3D* ptP) const
    {
		//std::vector<float> d(m_vertices.size());
		//std::transform(m_vertices.begin(), m_vertices.end(), d.begin(),[&line](const VertData& v)
		//{
		//	QVector3D q = v;
		//	float f = line.projection(q);
		//	if (f < 0) return std::numeric_limits<float>::infinity();
		//	return (line.point(f) - q).length();
		//}
		//);
		//size_t ind = size_t(std::min_element(d.begin(), d.end()) - d.begin());
		//return m_vertices[ind];

        std::vector<QVector3D> ipt(m_indices.size() / 3);


		float minr = std::numeric_limits<float>::infinity();

        for (int i = 0; i != m_indices.size() / 3; ++i)
        {
			QVector3D pt;
            float r = sprivRayTriIntersect(line, m_vertices[m_indices[i * 3 + 0]] , m_vertices[m_indices[i * 3 + 1]],  m_vertices[m_indices[i * 3 + 2]], &pt);
			if (minr > r)
			{
				minr = r;
				*ptP = pt;
			}

        }

		return !isinf(minr);
    }

    bool empty() const
    {
        return m_vertices.size() == 0;
    }


    
    void draw(const QMatrix4x4& mvp)
    {
        m_meshShader.bind();
        m_meshShader.setMVP(mvp);
        auto shader = m_meshShader.get();
        m_vBuff.bind();
        m_iBuff.bind();

        int vp = shader->attributeLocation("a_xyz");
        shader->enableAttributeArray(vp);
        shader->setAttributeBuffer(vp, GL_FLOAT, 0, 3, sizeof(VertData));
        

        int vc = shader->attributeLocation("a_rgb");
        shader->enableAttributeArray(vc);
        shader->setAttributeBuffer(vc, GL_FLOAT, 3 * sizeof(float), 3, sizeof(VertData));

        
        //glVertexAttribPointer(vc, 3, GL_UNSIGNED_INT8_NV, false, 3 * sizeof(uint8_t), NULL);
        glDrawElements(GL_TRIANGLES, m_iBuff.size() / sizeof(uint32_t), GL_UNSIGNED_INT, NULL);

        m_vBuff.release();
        m_iBuff.release();

        // Clean up state machine
        shader->disableAttributeArray(vp);
        shader->disableAttributeArray(vc);
        
    }

private:



    static float sprivRayTriIntersect(const QLine3D& line, const QVector3D& t0, const QVector3D& t1, const QVector3D& t2, QVector3D* iptP)
    {
        QVector3D& ipt = *iptP;
        
        static const float thr = 1e-9f;
        static const float inf = std::numeric_limits<float>::infinity();

                                        // get triangle edge vectors and plane normal
        QVector3D u = t1 - t0;
        QVector3D v = t2 - t0;
        
		QVector3D n = QVector3D::crossProduct(u, v);
        if (n.lengthSquared() == 0)             // triangle is degenerate
            return inf;                  // do not deal with this case

        QVector3D w0 = line.p1() - t0;
        
        float b = QVector3D::dotProduct(n, line.direction());;
        if (fabs(b) < thr) {     // ray is  parallel to triangle plane
            return inf;
        }
        float a = -QVector3D::dotProduct(n, line.p1() - t0);;
        // get intersect point of ray with triangle plane
        float r = a / b;
        if (r < 0.0)                    // ray goes away from triangle
            return inf;                   // => no intersect
                                        // for a segment, also test if (r > 1.0) => no intersect

        ipt = line.point(r);            // intersect point of ray and plane
		
                                        // is I inside T?

        float uu = QVector3D::dotProduct(u, u);
        float uv = QVector3D::dotProduct(u, v);
        float vv = QVector3D::dotProduct(v, v);
        QVector3D w = ipt - t0;
        float wu = QVector3D::dotProduct(w, u);
        float wv = QVector3D::dotProduct(w, v);
        float d = uv * uv - uu * vv;

        // get and test parametric coords
        float s, t;
        s = (uv * wv - vv * wu) / d;
        if (s < 0.0 || s > 1.0)         // I is outside T
            return inf;
        t = (uv * wu - uu * wv) / d;
        if (t < 0.0 || (s + t) > 1.0)  // I is outside T
            return inf;

        return r;                       // I is in T
    }





};



class MeshArray
{
    std::map<int,Mesh*> m_meshArr;
public:

    int allocate(Mesh*& p)
    {
        int token  =int( m_meshArr.size());
        
        m_meshArr.emplace(token,new Mesh());

        p = m_meshArr[token];


        return token;
    }
    std::vector<int> getTokenList()const
    {
        std::vector<int> t;
        
        for (auto it = m_meshArr.begin(); it != m_meshArr.end(); ++it) 
            t.push_back(it->first);
        return t;
    }
    Mesh* getMesh(int token)
    {
        if (!m_meshArr.count(token))
            return nullptr;
        return m_meshArr[token];
    }


    void releaseToken(int token)
    {
        delete  m_meshArr[token];
        m_meshArr.erase(token);
    }

    static MeshArray& i()
    {
        static MeshArray    instance; // Guaranteed to be destroyed.

        return instance;
    }
private:
    MeshArray() {}                    // Constructor? (the {} brackets) are needed here.


    

public:
    MeshArray(MeshArray const&) = delete;
    ~MeshArray()
    {
        for (auto i : m_meshArr)
            delete i.second;
    }
    void operator=(MeshArray const&) = delete;

};

#endif // MESH_H
