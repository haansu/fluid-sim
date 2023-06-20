#pragma once

// GLM
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
//

// Vulkan
#include <vulkan/vulkan_core.h>
//

// STL
#include <array>
//

struct Particle {
	glm::vec3 pos;
	glm::vec3 vel;
	glm::vec4 color;

	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions();
};
