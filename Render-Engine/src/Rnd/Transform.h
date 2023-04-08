#pragma once

#include <Defs.h>

#include <glm/vec3.hpp>

NAMESPACE_START_SCOPE_RND

struct Transform {
	glm::vec3 translate{ 0.0f };
	glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
	glm::vec3 rotate{};
};

NAMESPACE_END_SCOPE_RND