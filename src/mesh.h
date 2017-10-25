#ifndef MESH_H
#define MESH_H

#include <QtOpenGL/QtOpenGL>
#include <QVector3D>
#include <algorithm>
#include <vector>
#include <map>
#include <array>

class Mesh
{
public:
	Mesh() {}
	void set(const std::vector<QVector3D>&& v, const std::vector<GLuint>&& i)
	{
		m_vertices = v;
		m_indices = i;

	}

	std::array<QVector3D,2> closest2ray(const QVector3D& c,const QVector3D& ray) const
	{
		std::vector<QVector3D> ipt(m_indices.size() / 3);
		std::vector<QVector3D> npt(m_indices.size() / 3);
		std::vector<float> r(m_indices.size() / 3);
		
		for (int i = 0; i != m_indices.size() / 3; ++i)
		{
			r[i] = sprivRayTriIntersect(c, ray, m_vertices[m_indices[i * 3 + 0]] , m_vertices[m_indices[i * 3 + 1]],  m_vertices[m_indices[i * 3 + 2]], &(ipt[i]), &(npt[i]));

		}
		int ind = (std::min_element(r.begin(), r.end()) - r.begin());
		std::array<QVector3D, 2> o;
		if (isinf(r[ind]))
			o = { QVector3D() ,QVector3D() };
		else
		{
			o = { ipt[ind] ,npt[ind] };
			o[1].normalize();
		}
		return o;
	}

	bool empty() const
	{
		return m_vertices.size() == 0;
	}


	float xmin() const { return std::min_element(m_vertices.begin(), m_vertices.end(), [](const QVector3D& va, const QVector3D& vb) {return va.x() < vb.x(); })->x(); }
    float ymin() const { return std::min_element(m_vertices.begin(), m_vertices.end(), [](const QVector3D& va, const QVector3D& vb) {return va.y() < vb.y(); })->y(); }
    float zmin() const { return std::min_element(m_vertices.begin(), m_vertices.end(), [](const QVector3D& va, const QVector3D& vb) {return va.z() < vb.z(); })->z(); }
    float xmax() const { return std::max_element(m_vertices.begin(), m_vertices.end(), [](const QVector3D& va, const QVector3D& vb) {return va.x() < vb.x(); })->x(); }
    float ymax() const { return std::max_element(m_vertices.begin(), m_vertices.end(), [](const QVector3D& va, const QVector3D& vb) {return va.y() < vb.y(); })->y(); }
    float zmax() const { return std::max_element(m_vertices.begin(), m_vertices.end(), [](const QVector3D& va, const QVector3D& vb) {return va.z() < vb.z(); })->z(); }

	const QVector3D* vertData()const{		return m_vertices.data();	}
	const GLuint* indData()  const{ return m_indices.data(); }
	size_t vertSize()const { return m_vertices.size(); }
	size_t indSize() const { return m_indices.size(); }

	


private:
    std::vector<QVector3D> m_vertices;
    std::vector<GLuint> m_indices;//concatenated tri indices


	static float sprivRayTriIntersect(const QVector3D& r0,const QVector3D& rd, const QVector3D& t0, const QVector3D& t1, const QVector3D& t2, QVector3D* iptP,  QVector3D* nptP)
	{
		QVector3D& ipt = *iptP;
		QVector3D& n = *nptP;
		static const float thr = 1e-9f;
		static const float inf = std::numeric_limits<float>::infinity();

										// get triangle edge vectors and plane normal
		QVector3D u = t1 - t0;
		QVector3D v = t2 - t0;
		
		n = QVector3D::crossProduct(u, v);
		if (n.lengthSquared() == 0)             // triangle is degenerate
			return inf;                  // do not deal with this case

		QVector3D w0 = r0 - t0;
		
		float b = QVector3D::dotProduct(n, rd);;
		if (fabs(b) < thr) {     // ray is  parallel to triangle plane
			return inf;
		}
		float a = -QVector3D::dotProduct(n, r0 - t0);;
		// get intersect point of ray with triangle plane
		float r = a / b;
		if (r < 0.0)                    // ray goes away from triangle
			return inf;                   // => no intersect
										// for a segment, also test if (r > 1.0) => no intersect

		ipt = r0 + r * rd;            // intersect point of ray and plane

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
	std::map<int,Mesh> m_meshArr;
public:

	int allocate(Mesh*& p)
	{
		int token  =int( m_meshArr.size());
		m_meshArr.insert(std::pair<int, Mesh>(token, Mesh()));
		p = &m_meshArr[token];


		return token;
	}

	const Mesh* getToken(int token)
	{
		return &m_meshArr[token];
	}

	void releaseToken(int token)
	{
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
	void operator=(MeshArray const&) = delete;

};

#endif // MESH_H
