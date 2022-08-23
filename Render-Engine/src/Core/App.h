#pragma once

#include "../Defs.h"
#include "../Core.h"
#include "../Window/Window.h"

#include <vector>
#include <optional>

namespace rnd {

	struct QFamilyInd;

	class App final {
	public:
		RENDER_API void Run();

	private:
		void Init();
		void MainLoop();
		void CleanUp();
		void CreateInstance();

		void SetupDebugMsgr();
		void PopulateDebugMsgrCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		
		std::vector<const char*> GetReqExtensions();

		bool CheckValLayerSupport();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
			, VkDebugUtilsMessageTypeFlagsEXT messageType
			, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
			, void* pUserData);

		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		QFamilyInd FindQFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();
		void CreateSurface();
		void CreateSwapChain();

		bool CheckDeviceExtSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	private_var:
		// Note: Move initialization to constructor at some point
		Window m_Window { 800, 600, "Vulkan" };

		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		VkSurfaceKHR m_Surface;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImgs;

		VkQueue m_GraphicsQueue;
		VkQueue	m_PresentQueue;
	};

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