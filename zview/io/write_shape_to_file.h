#pragma once
#include "zview/common/common_types.h"
#include <algorithm>
#include <fstream>
namespace io
{
        void writeShapeToFile(const std::string &fn, const std::vector<Types::Shape>& shapes);

}