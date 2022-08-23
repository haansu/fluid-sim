#include "pch.h"
#include "App.h"
#include "Extra.h"
#include "../Core.h"
#include "../Window/Window.h"

#include <vector>
#include <optional>
#include <set>

namespace rnd {
		
	void App::Run() {
		Init();
		MainLoop();
		CleanUp();
	}

	void App::Init() {
		CreateInstance();
		SetupDebugMsgr();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
	}

	void App::MainLoop() {
		while (!m_Window.ShouldClose()) {
			glfwPollEvents();
		}
	}

	void App::CleanUp() {
		vkDestroyDevice(m_Device, nullptr);
		
		if (enableValLayers)
			DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessenger, nullptr);

		vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
		vkDestroyInstance(m_VkInstance, nullptr);

	}

	void App::CreateInstance() {
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
		

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};

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
			throw std::runtime_error("failed to create instance!");
		
		//
		// KEEP UNTIL LOGGING SYSTEM IN PLACE
		if (DEBUG_MESSGE) {
			std::cout << "\nAvailable extensions:\n";
			for (auto elem : extensions)
				std::cout << "\t" << elem << "\n";
		}
		
	}

	void App::SetupDebugMsgr() {
		if (!enableValLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMsgrCreateInfo(createInfo);

		if (CreateDebugUtilsMesageEXT(m_VkInstance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
			throw std::runtime_error("Failed to set up debug messenger!");
	}

	void App::PopulateDebugMsgrCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
	}

	std::vector<const char*> App::GetReqExtensions() {
		uint32_t glfwExtCount = 0;
		const char** glfwExts;

		glfwExts = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		std::vector<const char*> extensions(glfwExts, glfwExts + glfwExtCount);

		if (enableValLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	bool App::CheckValLayerSupport() {
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

	VKAPI_ATTR VkBool32 VKAPI_CALL App::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
		, VkDebugUtilsMessageTypeFlagsEXT messageType
		, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
		, void* pUserData) {
		std::cerr << "Validation layer: " << pCallbackData->pMessage << "\n";

		return VK_FALSE;
	}

	void App::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, nullptr);

		if (deviceCount == 0)
			throw std::runtime_error("Failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_VkInstance, &deviceCount, devices.data());

		for (const auto& elem : devices)
			if (IsDeviceSuitable(elem)) {
				m_PhysicalDevice = elem;
				break;
			}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Failed to find suitable GPU!");
	}

	bool App::IsDeviceSuitable(VkPhysicalDevice device) {
		QFamilyInd indices = FindQFamilies(device);

		bool extsSupported = CheckDeviceExtSupport(device);
		bool swapChainAdequate = false;

		if (extsSupported) {
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.IsComplete();
	}

	QFamilyInd App::FindQFamilies(VkPhysicalDevice device) {
		QFamilyInd ind;

		uint32_t qFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> qFamilies(qFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &qFamilyCount, qFamilies.data());

		uint16_t i = 0;

		for (const auto& elem : qFamilies) {
			if (elem.queueFlags & VK_QUEUE_GRAPHICS_BIT)
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

	void App::CreateLogicalDevice() {
		QFamilyInd ind = FindQFamilies(m_PhysicalDevice);

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
		} else
			createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");

		vkGetDeviceQueue(m_Device, ind.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, ind.presentFamily.value(), 0, &m_PresentQueue);
	}

	void App::CreateSurface() {
		if (glfwCreateWindowSurface(m_VkInstance, m_Window.GetGLFWwindowPointer(), nullptr, &m_Surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");
	}

	void App::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		
		// NOTE: LOOK HERE FOR 3D LATER
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImagecCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QFamilyInd ind = FindQFamilies(m_PhysicalDevice);
		uint32_t qFamilyInds[] = { ind.graphicsFamily.value(), ind.presentFamily.value() };

		if (ind.graphicsFamily != ind.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = qFamilyInds;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swap chain!");

		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImgs.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImgs.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	bool App::CheckDeviceExtSupport(VkPhysicalDevice device) {
		uint32_t extCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);

		std::vector<VkExtensionProperties> availableExt(extCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExt.data());

		std::set<std::string> reqExt(deviceExts.begin(), deviceExts.end());

		for (const auto& elem : availableExt)
			reqExt.erase(elem.extensionName);

		return reqExt.empty();
	}

	SwapChainSupportDetails App::QuerySwapChainSupport(VkPhysicalDevice device) {
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

	

}