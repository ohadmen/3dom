#pragma once
#include <string>
#include "zview/common/common_types.h"


namespace io
{
	Types::Mesh readstl(const std::string& filename);
	//void write3dd(const std::string& filename, std::vector<Types::Mesh*> meshes, std::vector<Types::Pcl*> pcls);
};

