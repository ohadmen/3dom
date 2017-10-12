#ifndef MESH_H
#define MESH_H

#include <QtOpenGL/QtOpenGL>
#include <QVector3D>
#include <algorithm>
#include <vector>
#include <map>

class Mesh
{
public:
	Mesh() {}
	void set(const std::vector<QVector3D>&& v, const std::vector<GLuint>&& i)
	{
		m_vertices = v;
		m_indices = i;

	}

	QVector3D closest2ray(const QVector3D& ray) const
	{
		std::vector<float> dist(m_vertices.size());
		for (int i = 0; i != m_vertices.size(); ++i)
			dist[i] = m_vertices[i].distanceToLine(QVector3D(), ray);
		int ind =  (std::min_element(dist.begin(), dist.end())- dist.begin());
		return m_vertices[ind];
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
    std::vector<GLuint> m_indices;




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
