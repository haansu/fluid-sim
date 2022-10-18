#include "pch.h"
#include "Vertex.h"
#include <vulkan/vulkan.h>

VkVertexInputBindingDescription Vertex::GetBindingDescription() {
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Vertex::GetAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attrDescriptions{};
	attrDescriptions[0].binding = 0;
	attrDescriptions[0].location = 0;
	attrDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attrDescriptions[0].offset = offsetof(Vertex, pos);
	attrDescriptions[1].binding = 0;
	attrDescriptions[1].location = 1;
	attrDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attrDescriptions[1].offset = offsetof(Vertex, color);

	return attrDescriptions;
}