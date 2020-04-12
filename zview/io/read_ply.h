#pragma once
#include <vector>
#include <memory>
#include "zview/common/common_types.h"

namespace
{
    struct PlyHeadeData
    {
        size_t nvertices;
        size_t nFaces;
        size_t nEdges;
    };
}

namespace io
{
    std::vector <Types::Shape> readPly(const char* fn);
}
