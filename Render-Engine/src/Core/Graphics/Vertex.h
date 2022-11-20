#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <array>

struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

struct Vertex {
	glm::vec3 pos;
	glm::vec4 color;
	glm::vec2 uv;
	glm::vec3 normal;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
};