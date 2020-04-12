#pragma once
#include "libcurie/drawables/drawable_base.h"
#include <algorithm>
#include <fstream>
namespace io
{
    void writePly(const char* fn, const std::vector<const DrawableBase*>& drawables)
    {
        std::ofstream fid(fn, std::ios::out | std::ios::binary);

        for (const auto& v : drawables)
                v->write(fid);
        fid.close();
    }
}