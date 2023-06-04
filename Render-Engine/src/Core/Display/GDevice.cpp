#include "pch.h"
#include "GDevice.h"

// Vulkan
#include <vulkan/vulkan.h>
//

// STL
#include <algorithm>
#include <array>
#include <limits>
#include <optional>
#include <set>
#include <vector>
#include <chrono>
//

namespace Render {

	GDevice::GDevice() {
		m_pWindow = new Window{ 800, 600, "Vulkan" };
		CreateInstance();
		SetupDebugMsgr();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPool();
	}
	
	GDevice::GDevice(Window& window) {
		m_pWindow = &window;
		CreateInstance();
		SetupDebugMsgr();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPool();
	}

	GDevice::~GDevice() {
		vkDestroyCommandPool(m_Device, m_ComandPool, nullptr);
		vkDestroyDevice(m_Device, nullptr);

		if (enableValLayers)
			DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMsgr, nullptr);

		vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
		vkDestroyInstance(m_VkInstance, nullptr);

		delete m_pWindow;
	}

	void GDevice::CreateInstance() {
		if (enableValLayers && !CheckValLayerSupport())
			throw std::runtime_error("Validation layers requested, but unavailable!");

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan FTM";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions = GetReqExtensions();

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();


		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

		if (enableValLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			PopulateDebugMsgrCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create instance!");

		//
		// KEEP UNTIL LOGGING SYSTEM IN PLACE
#ifdef FS_DEBUG
			std::cout << "\nAvailable extensions:\n";
			for (auto elem : extensions)
				std::cout << "\t" << elem << "\n";
#endif
	}

	void GDevice::SetupDebugMsgr() {
		if (!enableValLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMsgrCreateInfo(createInfo);

		if (CreateDebugUtilsMesageEXT(m_VkInstance, &createInfo, nullptr, &m_DebugMsgr) != VK_SUCCESS)
			throw std::runtime_error("Failed to set up debug messenger!");
	}

	void GDevice::CreateSurface() {
		if (glfwCreateWindowSurface(m_VkInstance, m_pWindow->GetGLFWwindowPointer(), nullptr, &m_Surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");
	}

	void GDevice::PopulateDebugMsgrCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = nullptr;
	}

	std::vector<const char*> GDevice::GetReqExtensions() {
		uint32_t glfwExtCount = 0;
		const char** glfwExts;

		glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		std::vector<const char*> extensions(glfwExts, glfwExts + glfwExtCount);

		if (enableValLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	bool GDevice::CheckValLayerSupport() {
		uint32_t layerCount;

		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}

			if (!layerFound)
				return false;
		}

		return true;
	}

	void GDevice::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("Failed to get GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

		for (const auto& elem : devices)
			if (IsDeviceSuitable(elem)) {
				m_PhysicalDevice = elem;
				break;
			}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to get suitable GPU!");
	}

	void GDevice::CreateLogicalDevice() {
		QFamilyInd ind = GetQFamilies(m_PhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> qCreateInfos;
		std::set<uint32_t> uniqueQFams = { ind.graphicsFamily.value(), ind.presentFamily.value() };

		float qPriority = 1.0f;

		for (uint32_t elem : uniqueQFams) {
			VkDeviceQueueCreateInfo qCreateInfo{};
			qCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			qCreateInfo.queueFamilyIndex = elem;
			qCreateInfo.queueCount = 1;
			qCreateInfo.pQueuePriorities = &qPriority;
			qCreateInfos.push_back(qCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		VkDeviceCreateInfo createInfo{};

		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(qCreateInfos.size());
		createInfo.pQueueCreateInfos = qCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExts.size());
		createInfo.ppEnabledExtensionNames = deviceExts.data();

		if (enableValLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");

		vkGetDeviceQueue(m_Device, ind.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, ind.graphicsFamily.value(), 0, &m_ComputeQueue);
		vkGetDeviceQueue(m_Device, ind.presentFamily.value(), 0, &m_PresentQueue);
	}

	void GDevice::CreateCommandPool() {
		QFamilyInd qFamInds = GetQFamilies(m_PhysicalDevice);

		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = qFamInds.graphicsFamily.value();

		if (vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_ComandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool!");
	}

	bool GDevice::IsDeviceSuitable(VkPhysicalDevice device) {
		QFamilyInd indices = GetQFamilies(device);

		bool extsSupported = CheckDeviceExtSupport(device);
		bool swapChainAdequate = false;

		if (extsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supFeatures;
		vkGetPhysicalDeviceFeatures(device, &supFeatures);

		return indices.IsComplete() && extsSupported && swapChainAdequate && supFeatures.samplerAnisotropy;
	}

	SwapChainSupportDetails GDevice::QuerySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		uint32_t formatCount;
		uint32_t presentModeCount;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

		if (formatCount) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
		}

		if (presentModeCount) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool GDevice::CheckDeviceExtSupport(VkPhysicalDevice device) {
		uint32_t extCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

		std::vector<VkExtensionProperties> availableExt(extCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExt.data());

		std::set<std::string> reqExt(deviceExts.begin(), deviceExts.end());

		for (const auto& elem : availableExt)
			reqExt.erase(elem.extensionName);

		return reqExt.empty();
	}

	uint32_t GDevice::GetMemType(uint32_t memTypeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProps);

		for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
			if ((memTypeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		throw std::runtime_error("Failed to get memory type");
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL GDevice::DebugCallback(
		  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
		, VkDebugUtilsMessageTypeFlagsEXT messageType
		, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
		, void* pUserData) {

		std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";
		return VK_FALSE;
	}

	QFamilyInd GDevice::GetQFamilies(VkPhysicalDevice device) {
		QFamilyInd ind;

		uint32_t qFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> qFamilies(qFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, qFamilies.data());

		uint16_t i = 0;

		for (const auto& elem : qFamilies) {
			if ((elem.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (elem.queueFlags & VK_QUEUE_COMPUTE_BIT))
				ind.graphicsFamily = i;

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

			if (presentSupport)
				ind.presentFamily = 1;

			if (ind.IsComplete())
				break;

			i++;
		}

		return ind;
	}

	[[nodiscard]] VkFormat GDevice::GetSupportedFormat(
		  const std::vector<VkFormat>& formats
		, VkImageTiling tiling
		, VkFormatFeatureFlags features) {

		for (VkFormat elem : formats) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, elem, &props);

			if ((props.linearTilingFeatures & features) == features && tiling == VK_IMAGE_TILING_LINEAR)
				return elem;
			else if ((props.optimalTilingFeatures & features) == features && tiling == VK_IMAGE_TILING_OPTIMAL)
				return elem;
		}

		throw std::runtime_error("Failed to get supported format!");
	}

	[[nodiscard]] VkCommandBuffer GDevice::BeginSTCommands() {
		VkCommandBufferAllocateInfo mallocInfo{};

		mallocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		mallocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		mallocInfo.commandPool = m_ComandPool;
		mallocInfo.commandBufferCount = 1;

		VkCommandBuffer cmdBuffer;
		vkAllocateCommandBuffers(m_Device, &mallocInfo, &cmdBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(cmdBuffer, &beginInfo);

		return cmdBuffer;
	}

	void GDevice::EndSTCommands(VkCommandBuffer cmdBuffer) {
		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_Device, m_ComandPool, 1, &cmdBuffer);
	}

}
