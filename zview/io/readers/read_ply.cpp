#include "read_ply.h"
#include <map>
#include <functional>
#include <regex>
#include <string>
#include <fstream>
#include <vector>
#include <fstream>
#include <string>

#include <iostream> //cout

namespace
{
    using ElemData = std::variant<std::vector<Types::VertData>, std::vector<Types::FaceIndx>, std::vector<Types::EdgeIndx>>;

    using ReaderFunc = std::function<ElemData(std::ifstream &ss, size_t count)>;

    std::map<std::string, ReaderFunc> getReaderMap()
    {
        std::map<std::string, ReaderFunc> map;
        //xyzrgba
        map["vertexpropertyfloatxpropertyfloatypropertyfloatzpropertyucharrpropertyuchargpropertyucharbpropertyuchara"] =
            [](std::ifstream &ss, size_t count) -> ElemData {
            std::vector<Types::VertData> v(count);
            ss.read((char *)(&v[0]), sizeof(v[0]) * count);
            return v;
        };
        //xyz
        map["vertexpropertyfloatxpropertyfloatypropertyfloatz"] =
            [](std::ifstream &ss, size_t count) -> ElemData {
            std::vector<Types::VertData> v(count);
            for (size_t i = 0; i != count; ++i)
                ss.read((char *)(&v[i]), sizeof(float) * 3);
            return v;
        };
        //edge
        map["edgepropertyintvertex1propertyintvertex2"] =
            [](std::ifstream &ss, size_t count) -> ElemData {
            std::vector<Types::EdgeIndx> v(count);
            for (size_t i = 0; i != count; ++i)
                ss.read((char *)&v[i], 2 * sizeof(int32_t));
            return v;
        };


        //face
        map["facepropertylistucharintvertex_indices"] =
            [](std::ifstream &ss, size_t count) -> ElemData {
            std::vector<Types::FaceIndx> v(count);
            uint8_t listsz;

            for (size_t i = 0; i != count; ++i)
            {
                ss.read((char *)&listsz, 1);
                if (listsz != 3)
                    throw std::runtime_error("support only tri meshes");
                ss.read((char *)&v[i], 3 * sizeof(int32_t));
            }

            return v;
        };

        return map;
    }

    struct ElementHeader
    {
        std::string type;
        size_t count;
        std::string signiture;
        ElementHeader(const std::string &type_, const std::string &count_, std::string &propList_) : type(type_), count(std::atoi(count_.c_str()))
        {
            signiture = type;
            for (size_t i = 0; i != propList_.size(); ++i)
            {
                if (!isspace(propList_[i]))
                {
                    signiture.push_back(propList_[i]);
                }
            }
        }
    };

    std::string readHeader(std::ifstream &ss)
    {
        std::string header;
        for (std::string line; std::getline(ss, line);)
        {
            line.erase(std::find_if(line.rbegin(), line.rend(), [](int ch) { return !std::isspace(ch); }).base(), line.end());
            header += line + " ";
            if (line.find("end_header") != std::string::npos)
                break;
        }

        return header;
    }
    std::vector<ElementHeader> getElementHeaders(const std::string &header)
    {
        std::vector<ElementHeader> elems;

        static const std::regex rx("element\\s+(\\w+)\\s+(\\d+)\\s*(.+?(?=element|end_header))");

        auto elemBegin =
            std::sregex_iterator(header.begin(), header.end(), rx);
        auto elemEnd = std::sregex_iterator();

        for (std::sregex_iterator i = elemBegin; i != elemEnd; ++i)
        {

            std::smatch m = *i;
            std::string prop = m[3];
            elems.push_back({m[1], m[2], prop});
        }

        return elems;
    }
    std::string getName(const std::string &header)
    {

        std::vector<ElementHeader> elems;

        static const std::regex rx("comment\\s+([^\\s]+)");

        auto itbeg =
            std::sregex_iterator(header.begin(), header.end(), rx);
        auto itend = std::sregex_iterator();
        std::string name;
        for (std::sregex_iterator i = itbeg; i != itend; ++i)
        {
            std::smatch m = *i;
            name = m[1];
            break;
        }

        return name;
    }

    Types::Shape elemArrayToshape(const std::vector<ElemData> &elems, const std::string &name)
    {

        const std::vector<Types::VertData> *verticesP = nullptr;
        const std::vector<Types::FaceIndx> *faceP = nullptr;
        const std::vector<Types::EdgeIndx> *edgesP = nullptr;

        for (const ElemData &a : elems)
        {
            if (std::holds_alternative<std::vector<Types::VertData>>(a))
            {
                if (verticesP)
                    throw std::runtime_error("data holds more than a single vertex data");
                verticesP = &std::get<std::vector<Types::VertData>>(a);
            }
            else if (std::holds_alternative<std::vector<Types::FaceIndx>>(a))
            {
                if (faceP)
                    throw std::runtime_error("data holds more than a single face data");
                faceP = &std::get<std::vector<Types::FaceIndx>>(a);
            }
            else if (std::holds_alternative<std::vector<Types::EdgeIndx>>(a))
            {
                if (edgesP)
                    throw std::runtime_error("data holds more than a single edge data");
                edgesP = &std::get<std::vector<Types::EdgeIndx>>(a);
            }
        }
        if (!verticesP)
            throw std::runtime_error("data doesnt hold vertex data");
        if (faceP && edgesP)
            throw std::runtime_error("data holds both mesh and edge data, currently not supported");

        if (faceP)
        {
            Types::Mesh obj(name);
            obj.v() = std::move(*verticesP);
            obj.f() = std::move(*faceP);
            return obj;
        }
        else if (edgesP)
        {
            Types::Edges obj(name);
            obj.v() = std::move(*verticesP);
            obj.e() = std::move(*edgesP);
            return obj;
        }
        else
        {
            Types::Pcl obj(name);
            obj.v() = std::move(*verticesP);
            return obj;
        }
    }
    std::vector<ElemData> readElems(const std::vector<ElementHeader> &elemHeaders, std::ifstream &ss)
    {
        std::vector<ElemData> elems;
        static const auto readerMap = getReaderMap();
        for (const auto &h : elemHeaders)
        {
            auto it = readerMap.find(h.signiture);
            if (it == readerMap.end())
                throw std::runtime_error("Could no parse ply file: element type " + h.type + " with the following propery list is not upported: " + h.signiture);
            const ReaderFunc &reader = it->second;
            elems.push_back(reader(ss, h.count));
        }

        return elems;
    }

} // namespace

std::vector<Types::Shape> io::readPly(const std::string &fn)
{

    std::ifstream ss(fn, std::ios::in | std::ios::binary);
    if (ss.fail())
        throw std::runtime_error("failed to open " + std::string(fn));

    std::vector<Types::Shape> container;

    while (!ss.eof())
    {

        std::string header = readHeader(ss);
        if (header.empty())
            break;
        auto elemHeaders = getElementHeaders(header);
        std::string name = getName(header);
        if (name.empty())
        {
            std::regex rx("([^\\\\\\/]+?)(\\.[^.]*$|$)");
              auto elemBegin =
            std::sregex_iterator(fn.begin(), fn.end(), rx);
            int nmatches = std::distance(elemBegin,std::sregex_iterator());
            if(nmatches!=1)
            {
                name = "unknown";
            }
            else
            {
                name=(*elemBegin)[1];
            }

        }
            auto elems = readElems(elemHeaders, ss);
            Types::Shape obj = elemArrayToshape(elems, name);
            container.emplace_back(obj);
        }
        return container;
    }
