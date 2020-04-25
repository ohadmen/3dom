#pragma once
#include "zview/common/common_types.h"
#include <algorithm>
#include <fstream>
namespace io
{
    void writePly(const std::string& fn, const std::vector<Types::Shape>::const_iterator drawables,size_t n)
    {
        std::ofstream fid(fn, std::ios::out | std::ios::binary);

        // for (const auto& v : drawables)
        //         v->write(fid);
        fid.close();
    }
}