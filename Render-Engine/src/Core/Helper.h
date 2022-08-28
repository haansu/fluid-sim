#pragma once

#include <vector>

namespace rnd::Helper {

	[[nodiscard]] std::vector<char> ReadFile(const std::string& fileName);

}