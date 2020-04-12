#include "read_stl.h"
#include <fstream>
#include <cerrno>
#include <memory>
#include <cstring>



uint8_t privColData2ch(uint16_t cd, int ch)
{
	const uint16_t mask = (1 << 5) - 1;
    uint8_t v = uint8_t(((cd >> ch * 5) & mask)<<3);
	return v ;
}

bool privIsBinaryStl(std::ifstream* ifs)
{
	char buff[6];
	ifs->read(buff, 6);
	ifs->seekg(0);
	return std::strcmp(buff, "solid ") != 0; //if !=0 -> string are different --> binary --> return true
}

void privReadStlBin(std::ifstream* ifsP, Types::Mesh* meshP)
{
	Types::Mesh& m = *meshP;
	std::ifstream& ifs = *ifsP;
	// get file size
	ifs.seekg(0, std::ios::end);
	uint32_t fileSize = ifs.tellg();
	ifs.seekg(0, std::ios::beg);

	if (fileSize == 0) // avoid undefined behavior 
		throw std::runtime_error("empty file");


	// Load the triangle count from the .stl file
	ifs.ignore(80);
	uint32_t triCount;
	ifs.read(reinterpret_cast<char*>(&triCount), 4);

	// Verify that the file is the right size
	if (fileSize != 84 + triCount * 50)
		throw std::runtime_error("bad file size");

	// Extract vertices into an array of xyz, unsigned pairs
	//QVector<Vertex> verts(triCount * 3);

	m.v().resize(triCount * 3);


	// Dummy array, because readRawData is faster than skipRawData
	std::unique_ptr<char> buffer(new char[triCount * 50]);
	ifs.read(buffer.get(), triCount * 50);

	// Store vertices in the array, processing one triangle at a time.
	auto b = buffer.get() + 3 * sizeof(float);
	for (auto v = m.v().begin(); v != m.v().end(); v += 3)
	{
		// Load vertex data from .stl file into vertices
		uint16_t coldata = *reinterpret_cast<uint16_t*>(b + 9 * sizeof(float));

		uint8_t rgb[3] = { privColData2ch(coldata,0) ,privColData2ch(coldata,1),privColData2ch(coldata,2) };
		for (unsigned i = 0; i < 3; ++i)
		{
			memcpy(&v[i], b, 3 * sizeof(float));
			v[i].r = rgb[0];
			v[i].g = rgb[1];
			v[i].b = rgb[2];
			b += 3 * sizeof(float);
		}


		// Skip face attribute and next face's normal vector
		b += 3 * sizeof(float) + sizeof(uint16_t);
	}


	m.f().resize(triCount);
	for (size_t i = 0; i != triCount * 3; ++i)
		m.f()[i / 3][i % 3] = i;



}

void privReadStlAscii(std::ifstream* ifsP, Types::Mesh* meshP)
{
	throw std::runtime_error("ASCII STL are unsopported");
}

Types::Mesh io::readstl(const std::string& filename)
{
	Types::Mesh obj;
	std::ifstream ifs(filename, std::ios::binary);

	if (!ifs)
		throw std::runtime_error(filename + ": " + std::strerror(errno));
	privIsBinaryStl(&ifs) ? privReadStlBin(&ifs, &obj) : privReadStlAscii(&ifs, &obj);
	return obj;
}
