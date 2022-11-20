#pragma once

// Vulkan
#include <vulkan/vulkan_core.h>
//

// STL
#include <vector>
//

#include "../../Defs.h"
#include "../../Window/Window.h"
#include "../Extra.h"
#include "../../Core.h"


namespace Render {

	/// <summary>
	/// Graphics Device
	/// </summary>
	class GDevice {
	public:
		NO_COPY(GDevice);
		NO_MOVE(GDevice);

		GDevice();
		GDevice(Window& window);
		~GDevice();
		
		inline VkCommandPool&		GetCommandPool() { return m_ComandPool; }
		inline VkDevice&			GetDevice() { return m_Device; }
		inline VkPhysicalDevice&	GetPhysicalDevice() { return m_PhysicalDevice; }
		inline VkSurfaceKHR&		GetSurface() { return m_Surface; }
		inline VkQueue&				GetGraphicsQueue() { return m_GraphicsQueue; }
		inline VkQueue&				GetPresentQueue() { return m_PresentQueue; }
		inline Window*				GetWindow() { return m_pWindow; }

		/// <summary>
		/// Returns the a command buffer for single time commands
		/// </summary>
		/// <returns>VkCommandBuffer</returns>
		[[nodiscard]] VkCommandBuffer BeginSTCommands();

		/// <summary>
		/// End single time commands
		/// </summary>
		/// <param name="cmdBuffer">VkCommandBuffer</param>
		void EndSTCommands(VkCommandBuffer cmdBuffer);

		/// <summary>
		/// Checks for Vulkan device swap chain support
		/// </summary>
		/// <param name="device">VkPhysicalDevice</param>
		/// <returns>SwapChainSupportDetails</returns>
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
		
		/// <summary>
		/// Find Queue Families
		/// </summary>
		/// <param name="device"></param>
		/// <returns>QFamilyInd</returns>
		QFamilyInd GetQFamilies(VkPhysicalDevice device);

		/// <summary>
		/// Gets Vulkan supported Formats
		/// </summary>
		/// <param name="formats">const std::vector<VkFormat>&</param>
		/// <param name="tiling">VkImageTiling</param>
		/// <param name="features">VkFormatFeatureFlags</param>
		/// <returns></returns>
		[[nodiscard]] VkFormat GetSupportedFormat(
			  const std::vector<VkFormat>& formats
			, VkImageTiling tiling
			, VkFormatFeatureFlags features
		);

		/// <summary>
		/// Get Memory Type
		/// </summary>
		/// <param name="memTypeFilter">uint32_t</param>
		/// <param name="properties">VkMemoryPropertyFlags</param>
		/// <returns></returns>
		uint32_t GetMemType(uint32_t memTypeFilter, VkMemoryPropertyFlags properties);
	
	private:
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateCommandPool();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtSupport(VkPhysicalDevice device);
		
		void CreateInstance();
		void SetupDebugMsgr();
		void CreateSurface();
		
		void PopulateDebugMsgrCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		std::vector<const char*> GetReqExtensions();
		
		bool CheckValLayerSupport();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
			, VkDebugUtilsMessageTypeFlagsEXT messageType
			, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
			, void* pUserData
		);

	private_var:
		Window* m_pWindow;
		
		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_DebugMsgr;
		VkCommandPool m_ComandPool;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		VkSurfaceKHR m_Surface;
		VkQueue m_GraphicsQueue;
		VkQueue	m_PresentQueue;
	};

}
