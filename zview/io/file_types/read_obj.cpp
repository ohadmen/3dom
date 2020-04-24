#include "read_obj.h"
#include <fstream>
#include <sstream>
#include <string>
#include <numeric>

namespace
{
    std::stringstream  getline(std::ifstream& ifs)
    {
        std::string line;
        std::getline(ifs,line);
        std::stringstream st(line);
        return st;
        
    }
    
}
std::vector<std::pair<std::string, Types::Shape>> io::readObj(const char *fn)
{
        std::ifstream ifs(fn, std::ios::in | std::ios::binary);
    if (ifs.fail()) throw
        std::runtime_error("failed to open " + std::string(fn));

    
    Types::Mesh mesh;
    Types::Edges edges;
    

    std::vector<Types::VertData> allpoints;
    while (true)
    {   
        if (ifs.eof())
            break;
        auto line = getline(ifs);
        std::string type;
        line >> type;
        if(type.empty() || type[0]=='#')
            continue;
        
        if(type=="v")
        {
            Types::VertData v;
            float rgb[3];
            line >> v.x;
            line >> v.y;
            line >> v.z;
            line >> rgb[0];
            line >> rgb[1];
            line >> rgb[2];
            v.r=rgb[0]*255;
            v.g=rgb[1]*255;
            v.b=rgb[2]*255;
            v.a=200;
            allpoints.push_back(v);
        }
        else if(type=="f")
        {
            
            //support case that we have more than 3 vetrices - triagulize it
            uint32_t f0;
            uint32_t f12[2];

            line >> f0;
            line >> f12[0];
            while(line>>f12[1])
            {
                //INDICES are one based
                mesh.f().push_back({f0-1,f12[0]-1,f12[1]-1});
                f12[0]=f12[1];

            }
            
        }
        else if(type=="e")
        {
            Types::EdgeIndx e;
            line >> e[0];
            line >> e[1];
            edges.e().push_back(e);
        }
        


    }
    std::vector<bool> usedPoint(allpoints.size());
    for(const auto& a: mesh.f())
    {
        for(const auto& b:a)
            if(b>=0 && b<usedPoint.size())
                usedPoint[b]=true;
    }
    for(const auto& a: edges.e())
    {
        for(const auto& b:a)
        {
            if(b>=0 && b<usedPoint.size())
                usedPoint[b]=true;
        }
            
    }


    std::vector <std::pair<std::string,Types::Shape>> container;
    size_t unused = std::accumulate(usedPoint.begin(),usedPoint.end(),size_t(0),[](size_t s,bool v){return s+(v==0);});
    if(unused!=0)
    {
        Types::Pcl pcl;
        for(size_t i=0;i!=usedPoint.size();++i)
            if(!usedPoint[i])
                pcl.v().push_back(allpoints[i]);
        
        container.push_back({"pcl",pcl});
    }

    if(edges.e().size()!=0)
    {
        edges.v()=allpoints;
        container.push_back({"edges",edges});
    }
    if(mesh.f().size()!=0)
    {
        mesh.v()=allpoints;
        container.push_back({"mesh",mesh});
    }

    return container;
    
}
