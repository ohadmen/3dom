#ifndef LOADER_H
#define LOADER_H

#include <QThread>
#include <future>


//#include "vertex.h"
#include "mesh.h"

class Loader : public QThread
{
    Q_OBJECT

		struct Vertex
	{
		Vertex() {}
		Vertex(float x, float y, float z) : x(x), y(y), z(z) {}

		GLfloat x, y, z;
		GLuint i = 0;


		bool operator!=(const Vertex& rhs) const
		{
			return x != rhs.x || y != rhs.y || z != rhs.z;
		}
		bool operator<(const Vertex& rhs) const
		{
			if (x != rhs.x)    return x < rhs.x;
			else if (y != rhs.y)    return y < rhs.y;
			else if (z != rhs.z)    return z < rhs.z;
			else                    return false;
		}
		operator QVector3D()		{			return QVector3D(x, y, z);		}
	};
		
	


public:
    explicit Loader(QObject* parent, const QString& filename, bool is_reload)
		: QThread(parent), filename(filename), is_reload(is_reload)
	{
		// Nothing to do here
	}

	void run()
	{
		int token = load_stl();

		if (token == -1)
		{
			emit error_empty_mesh();

		}
		else
		{
			emit got_mesh(token, is_reload);
			emit loaded_file(filename);
		}


	}
    

protected:
	int load_stl()
	{
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly))
		{
			emit error_missing_file();
			return -1;
		}

		// First, try to read the stl as an ASCII file
		if (file.read(6) == "solid ")
		{
			file.readLine(); // skip solid name
			const auto line = file.readLine().trimmed();
			if (line.startsWith("facet") ||
				line.startsWith("endsolid"))
			{
				file.seek(0);
				return read_stl_ascii(file);
			}
			confusing_stl = true;
		}
		else
		{
			confusing_stl = false;
		}

		// Otherwise, skip the rest of the header material and read as binary
		file.seek(0);
		return read_stl_binary(file);
	}

	int read_stl_binary(QFile& file)
	{
		QDataStream data(&file);
		data.setByteOrder(QDataStream::LittleEndian);
		data.setFloatingPointPrecision(QDataStream::SinglePrecision);

		// Load the triangle count from the .stl file
		file.seek(80);
		uint32_t tri_count;
		data >> tri_count;

		// Verify that the file is the right size
		if (file.size() != 84 + tri_count * 50)
		{
			emit error_bad_stl();
			return -1;
		}

		// Extract vertices into an array of xyz, unsigned pairs
		QVector<Vertex> verts(tri_count * 3);

		// Dummy array, because readRawData is faster than skipRawData
		std::unique_ptr<uint8_t> buffer(new uint8_t[tri_count * 50]);
		data.readRawData((char*)buffer.get(), tri_count * 50);

		// Store vertices in the array, processing one triangle at a time.
		auto b = buffer.get() + 3 * sizeof(float);
		for (auto v = verts.begin(); v != verts.end(); v += 3)
		{
			// Load vertex data from .stl file into vertices
			for (unsigned i = 0; i < 3; ++i)
			{
				memcpy(&v[i], b, 3 * sizeof(float));
				b += 3 * sizeof(float);
			}

			// Skip face attribute and next face's normal vector
			b += 3 * sizeof(float) + sizeof(uint16_t);
		}

		if (confusing_stl)
		{
			emit warning_confusing_stl();
		}

		return mesh_from_verts(tri_count, verts);
	}

	int read_stl_ascii(QFile& file)
	{
		file.readLine();
		uint32_t tri_count = 0;
		QVector<Vertex> verts(tri_count * 3);

		bool okay = true;
		while (!file.atEnd() && okay)
		{
			const auto line = file.readLine().simplified();
			if (line.startsWith("endsolid"))
			{
				break;
			}
			else if (!line.startsWith("facet normal") ||
				!file.readLine().simplified().startsWith("outer loop"))
			{
				okay = false;
				break;
			}

			for (int i = 0; i < 3; ++i)
			{
				auto line = file.readLine().simplified().split(' ');
				if (line[0] != "vertex")
				{
					okay = false;
					break;
				}
				const float x = line[1].toFloat(&okay);
				const float y = line[2].toFloat(&okay);
				const float z = line[3].toFloat(&okay);
				verts.push_back(Vertex(x, y, z));
			}
			if (!file.readLine().trimmed().startsWith("endloop") ||
				!file.readLine().trimmed().startsWith("endfacet"))
			{
				okay = false;
				break;
			}
			tri_count++;
		}

		if (okay)
		{
			return mesh_from_verts(tri_count, verts);
		}
		else
		{
			emit error_bad_stl();
			return -1;
		}
	}



signals:
    void loaded_file(QString filename);
    void got_mesh(int token, bool is_reload);

    void error_bad_stl();
    void error_empty_mesh();
    void warning_confusing_stl();
    void error_missing_file();

private:

	static void parallel_sort(Vertex* begin, Vertex* end, int threads)
	{
		if (threads < 2 || end - begin < 2)
		{
			std::sort(begin, end);
		}
		else
		{
			const auto mid = begin + (end - begin) / 2;
			if (threads == 2)
			{
				auto future = std::async(parallel_sort, begin, mid, threads / 2);
				std::sort(mid, end);
				future.wait();
			}
			else
			{
				auto a = std::async(std::launch::async, parallel_sort, begin, mid, threads / 2);
				auto b = std::async(std::launch::async, parallel_sort, mid, end, threads / 2);
				a.wait();
				b.wait();
			}
			std::inplace_merge(begin, mid, end);
		}
	}

	int mesh_from_verts(uint32_t tri_count, QVector<Vertex>& verts)
	{
		// Save indicies as the second element in the array
		// (so that we can reconstruct triangle order after sorting)
		for (int i = 0; i != tri_count * 3; ++i)
		{
			verts[i].i = i;
		}

		// Check how many threads the hardware can safely support. This may return
		// 0 if the property can't be read so we shoud check for that too.
		auto threads = std::thread::hardware_concurrency();
		if (threads == 0)
		{
			threads = 8;
		}


		// Sort the set of vertices (to deduplicate)
		parallel_sort(verts.begin(), verts.end(), threads);

		// This vector will store triangles as sets of 3 indices
		std::vector<GLuint> indices(tri_count * 3);

		// Go through the sorted vertex list, deduplicating and creating
		// an indexed geometry representation for the triangles.
		// Unique vertices are moved so that they occupy the first vertex_count
		// positions in the verts array.
		int vertex_count = 0;
		for (auto v : verts)
		{
			if (!vertex_count || v != verts[vertex_count - 1])
			{
				verts[vertex_count++] = v;
			}
			indices[v.i] = vertex_count - 1;
		}
		verts.resize(vertex_count);

		std::vector<QVector3D> flat_verts(verts.size());
		
		for (int i=0;i!=verts.size();++i)
		{
			flat_verts[i] = verts[i];
		}
		Mesh* p;
		int token =  MeshArray::i().allocate(p);
		p->set(std::move(flat_verts), std::move(indices));
		return token;
	}


    const QString filename;
    bool is_reload;

    /*  Used to warn on binary STLs that begin with the word 'solid'" */
    bool confusing_stl;

};

#endif // LOADER_H
