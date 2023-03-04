#pragma once

#include <glm/mat4x4.hpp>

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	glm::vec4 lightDir;
	glm::vec4 lightColor;
	glm::vec4 lightAmbient;
	glm::vec4 lightDiffuse;
};