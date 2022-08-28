#include "pch.h"
#include "Helper.h"

#include <fstream>
#include <vector>
#include <string>

namespace rnd::Helper {

	[[nodiscard]] std::vector<char> ReadFile(const std::string& fileName) {
		std::ifstream file(fileName, std::ios::ate | std::ios::binary);

		if (!file.is_open())
			throw std::runtime_error("Failed to open file!");

		size_t fileSize = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

}