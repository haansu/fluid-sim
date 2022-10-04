#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

struct Vertex {
	glm::vec2 pos;
	glm::vec4 color;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};

const std::vector<Vertex> vertices = {
		  {{0.0f, -0.5f}	, {1.0f, 0.0f, 0.0f, 0.5f}}
		, {{0.5f, 0.5f}		, {0.0f, 0.0f, 1.0f, 0.3f}}
		, {{-0.5f, 0.5f}	, {0.0f, 1.0f, 0.0f, 1.0f}}
};
