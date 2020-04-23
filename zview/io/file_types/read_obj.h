#pragma once
#include <vector>
#include "zview/common/common_types.h"


namespace io
{
    std::vector <std::pair<std::string,Types::Shape>> readObj(const char* fn);
}
