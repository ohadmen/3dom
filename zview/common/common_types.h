#pragma once
#include <QtGui/qvector3d.h>
#include <vector>
#include <array>
#include <variant>

namespace Types
{

class Roi3d
{
	std::array<float, 2> m_x;
	std::array<float, 2> m_y;
	std::array<float, 2> m_z;

public:
	Roi3d() : m_x{0, 0}, m_y{0, 0}, m_z{0, 0} {}
	Roi3d(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax) : m_x{xmin, xmax}, m_y{ymin, ymax}, m_z{zmin, zmax} {}
	void operator|=(const Roi3d &other)
	{
		m_x[0] = std::min(m_x[0], other.m_x[0]);
		m_x[1] = std::max(m_x[1], other.m_x[1]);
		m_y[0] = std::min(m_y[0], other.m_y[0]);
		m_y[1] = std::max(m_y[1], other.m_y[1]);
		m_z[0] = std::min(m_z[0], other.m_z[0]);
		m_z[1] = std::max(m_z[1], other.m_z[1]);
	}
	float rangex() const { return m_x[1] - m_x[0]; }
	float rangey() const { return m_y[1] - m_y[0]; }
	float rangez() const { return m_z[1] - m_z[0]; }

	float midx() const { return (m_x[0] + m_x[1]) / 2; }
	float midy() const { return (m_y[0] + m_y[1]) / 2; }
	float midz() const { return (m_z[0] + m_z[1]) / 2; }
	QVector3D mid() { return {midx(), midy(), midz()}; }

	float minx() const { return m_x[0]; }
	float maxx() const { return m_x[1]; }
	float miny() const { return m_y[0]; }
	float maxy() const { return m_y[1]; }
	float minz() const { return m_z[0]; }
	float maxz() const { return m_z[1]; }
};

struct VertData
{
	float x;
	float y;
	float z;
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;

	VertData() : x(0), y(0), z(0), r(0), g(0), b(0), a(255) {}
	VertData(float x_, float y_, float z_, uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0, uint8_t a_ = 255) : x(x_), y(y_), z(z_), r(r_), g(g_), b(b_), a(a_) {}

	operator QVector3D() const { return QVector3D(x, y, z); }
	bool operator!=(const VertData &rhs) const
	{
		return x != rhs.x || y != rhs.y || z != rhs.z;
	}
	bool operator<(const VertData &rhs) const
	{
		if (x != rhs.x)
			return x < rhs.x;
		else if (y != rhs.y)
			return y < rhs.y;
		else if (z != rhs.z)
			return z < rhs.z;
		else
			return false;
	}
};
using FaceIndx = std::array<uint32_t, 3>;
using EdgeIndx = std::array<uint32_t, 2>;

class Pcl
{
protected:
	std::vector<Types::VertData> m_v;

public:
	std::vector<VertData> &v() { return m_v; }
	const std::vector<VertData> &v() const { return m_v; }

	Roi3d get3dbbox() const
	{
		if (m_v.size() == 0)
		{
			static const float e = 1e-3;
			return Types::Roi3d(-e, e, -e, e, -e, e);
		}
		else if (m_v.size() == 1)
		{
			static const float e = 1e-3;
			return Types::Roi3d(m_v[0].x - e, m_v[0].x + e, m_v[0].y - e, m_v[0].y + e, m_v[0].z - e, m_v[0].z + e);
		}
		else
		{
			auto xmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData &a, const Types::VertData &b) { return a.x < b.x; });
			auto ymm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData &a, const Types::VertData &b) { return a.y < b.y; });
			auto zmm = std::minmax_element(m_v.begin(), m_v.end(), [](const Types::VertData &a, const Types::VertData &b) { return a.z < b.z; });
			return Types::Roi3d(xmm.first->x, xmm.second->x, ymm.first->y, ymm.second->y, zmm.first->z, zmm.second->z);
		}
	}
};

class Mesh : public Pcl
{
protected:
	std::vector<FaceIndx> m_f;

public:
	std::vector<FaceIndx> &f() { return m_f; }
	const std::vector<FaceIndx> &f() const { return m_f; }
};

class Edges : public Pcl
{
protected:
	std::vector<EdgeIndx> m_e;

public:
	std::vector<EdgeIndx> &e() { return m_e; }
	const std::vector<EdgeIndx> &e() const { return m_e; }
};
using Shape = std::variant<Pcl, Edges, Mesh>;
} // namespace Types
