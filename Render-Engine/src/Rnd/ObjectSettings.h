#pragma once

// STL
#include <string>
//

// GLM
#include <glm/vec4.hpp>
//

// Core
#include "Transform.h"
#include <Defs.h>
//

NAMESPACE_START_SCOPE_RND

struct ObjectSettings {
	ObjectSettings() {
		path = "";
		color = glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f };

		id = s_IdCount;
		s_IdCount++;
	}

	std::string path;
	glm::vec4 color;
	Transform transform;
	uint64_t id;

	static uint64_t s_IdCount;
};

NAMESPACE_END_SCOPE_RND
