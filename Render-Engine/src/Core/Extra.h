#pragma once

// Vulkan
#include <vulkan/vulkan_core.h>
//

// STL
#include <optional>
#include <vector>
//

// GLM
#include <glm/mat4x4.hpp>
//

namespace Render {

#ifdef FS_DEBUG
	constexpr bool enableValLayers = true;
#else
	constexpr bool enableValLayers = false;
#endif

	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	const std::vector<const char*> deviceExts = { "VK_KHR_swapchain" };

	VkResult CreateDebugUtilsMesageEXT(VkInstance instance
		, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
		, const VkAllocationCallbacks* pAllocator
		, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance
		, VkDebugUtilsMessengerEXT debugMessenger
		, const VkAllocationCallbacks* pAllocator);

	struct QFamilyInd {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		inline bool IsComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct Trf {
		glm::vec3 translate{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotate{};

		glm::mat4 Model();
		glm::mat3 Normal();
	};

	struct PushConstantData {
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
		glm::vec4 color;
	};

}
