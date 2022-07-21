#pragma once

#include "../Defs.h"
#include "../Window/Window.h"
#include <vector>

namespace rnd {

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

		Window m_Window { 800, 600, "Vulkan" };

		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
	};
}