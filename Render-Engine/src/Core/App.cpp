#include "pch.h"
#include "App.h"
#include "Validation.h"
#include "../Core.h"
#include "../Window/Window.h"

#include <vector>
namespace rnd {
		
	void App::Run() {
		Init();
		MainLoop();
		CleanUp();
	}

	void App::Init() {
		CreateInstance();
		SetupDebugMsgr();
	}

	void App::MainLoop() {
		while (!m_Window.ShouldClose()) {
			glfwPollEvents();
		}
	}

	void App::CleanUp() {
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
		appInfo.apiVersion = VK_API_VERSION_1_2;

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
			std::cout << "Available extensions:\n";
			for (auto elem : extensions)
				std::cout << elem << "\n";
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

}