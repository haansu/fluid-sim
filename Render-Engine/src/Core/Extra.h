#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include <optional>

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

}