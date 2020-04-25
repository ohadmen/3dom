#pragma once
#include <vector>
#include "zview/common/common_types.h"


namespace io
{
    std::vector <Types::Shape> readPly(const std::string& fn);
}
