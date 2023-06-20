#include "pch.h"
#include "Particle.h"

#include <vulkan/vulkan.h>

VkVertexInputBindingDescription Particle::GetBindingDescription() {
	VkVertexInputBindingDescription bindingDesc{};
	bindingDesc.binding = 0;
	bindingDesc.stride = sizeof(Particle);
	bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDesc;
}

std::array<VkVertexInputAttributeDescription, 2> Particle::GetAttributeDescriptions() {
	std::array<VkVertexInputAttributeDescription, 2> attrDesc{};

	attrDesc[0].binding = 0;
	attrDesc[0].location = 0;
	attrDesc[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attrDesc[0].offset = offsetof(Particle, pos);

	attrDesc[1].binding = 0;
	attrDesc[1].location = 1;
	attrDesc[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attrDesc[1].offset = offsetof(Particle, color);

	return attrDesc;
}