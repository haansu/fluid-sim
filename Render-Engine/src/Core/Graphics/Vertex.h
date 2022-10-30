#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>
#include <array>

struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

struct Vertex {
	glm::vec2 pos;
	glm::vec4 color;
	glm::vec2 texPos;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();
};

const std::vector<Vertex> vertices = {
	  { {  0.5f, -0.5f },  { 1.0f, 0.0f, 0.0f, 1.0f },  { 1.0f, 0.0f } }
	, { {  0.5f,  0.5f },  { 0.0f, 1.0f, 0.0f, 1.0f },  { 0.0f, 0.0f } }
	, { { -0.5f,  0.5f },  { 0.0f, 0.0f, 1.0f, 1.0f },  { 0.0f, 1.0f } }
	, { { -0.5f, -0.5f },  { 1.0f, 0.0f, 1.0f, 1.0f },  { 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};
