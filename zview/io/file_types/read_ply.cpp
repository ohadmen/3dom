#define TINYPLY_IMPLEMENTATION
#include "read_ply.h"
#include <tinyply.h>
#include <fstream>

namespace
{
    struct PlyHeadeData
    {
        size_t nvertices;
        size_t nFaces;
        size_t nEdges;
    };
}

std::vector <std::pair<std::string,Types::Shape>> io::readPly(const char* fn)
{
    
    std::ifstream ss(fn, std::ios::in | std::ios::binary);
    if (ss.fail()) throw
        std::runtime_error("failed to open " + std::string(fn));

    std::vector <std::pair<std::string,Types::Shape>> container;
    
    while (true)
    {   
        
        tinyply::PlyFile file;

        file.parse_header(ss);
        if (ss.eof())
            break;
        std::string name;
        for (auto c : file.get_comments()) name += c + "/";
        name.pop_back();

        std::shared_ptr<tinyply::PlyData> vertices, faces, edges;
        try
        {
            vertices = file.request_properties_from_element("vertex", { "x", "y", "z","r","g","b","a" });
        }
        catch (const std::exception& e)
        {
         //no verts
        }
        try
        {
            faces = file.request_properties_from_element("face", { "vertex_indices" });
        }
        catch (const std::exception& e)
        {
        }
        try
        {

            edges = file.request_properties_from_element("edge", { "vertex1","vertex2" });
        }
        catch (const std::exception& e)
        {
        }

        file.read(ss);
        if (!vertices)
        {
            std::cout << "!vertices" << std::endl;
            continue;
        }
            
        

        
        if (faces)
        {
            Types::Mesh obj;
            obj.v().resize(vertices->count);
            std::memcpy(obj.v().data(), vertices->buffer.get(), vertices->buffer.size_bytes());

            obj.f().resize(faces->count); 
            std::memcpy(obj.f().data(), faces->buffer.get(), faces->buffer.size_bytes());
            container.emplace_back(std::make_pair(name,obj));

        }
        if (edges)
        {
            Types::Edges obj;
            obj.v().resize(vertices->count);
            std::memcpy(obj.v().data(), vertices->buffer.get(), vertices->buffer.size_bytes());

            obj.e().resize(edges->count);
            std::memcpy(obj.e().data(), edges->buffer.get(), edges->buffer.size_bytes());
            container.emplace_back(std::make_pair(name,obj));
        }
        if (!edges & !faces)
        {
            Types::Pcl obj;
            obj.v().resize(vertices->count);
            std::memcpy(obj.v().data(), vertices->buffer.get(), vertices->buffer.size_bytes());
            container.emplace_back(std::make_pair(name,obj));
        }

    }
    return container;

}
