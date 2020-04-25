#pragma once

#include <vector>
#include <string>
#include "zview/common/common_types.h"

namespace io
{
std::vector <Types::Shape> readFileList(const QStringList &files);
}