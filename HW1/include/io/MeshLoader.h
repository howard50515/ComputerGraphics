#pragma once

#include <string>

#include "core/Mesh.h"

namespace hw1 {

class MeshLoader {
public:
	static Mesh loadFromFile(const std::string& path);
};

}  // namespace hw1
