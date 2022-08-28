#pragma once

#include "../Core.h"

#include <vector>

namespace Render {

#ifdef FS_DEBUG
	constexpr bool enableValLayers = true;
#else
	constexpr bool enableValLayers = false;
#endif


	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

	const std::vector<const char*> deviceExts = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	VkResult CreateDebugUtilsMesageEXT(VkInstance instance
		, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
		, const VkAllocationCallbacks* pAllocator
		, VkDebugUtilsMessengerEXT* pDebugMessenger);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance
		, VkDebugUtilsMessengerEXT debugMessenger
		, const VkAllocationCallbacks* pAllocator);

}