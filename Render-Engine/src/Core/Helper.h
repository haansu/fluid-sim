#pragma once

#include <vector>

namespace Render::Helper {

	[[nodiscard]] std::vector<char> ReadFile(const std::string& fileName);

}
