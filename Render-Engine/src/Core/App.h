#pragma once

#include <vector>

#include <vulkan/vulkan_core.h>

#include "../Defs.h"

namespace Render {

	// Forward declared local stuff

	class Window;

	struct QFamilyInd;
	struct SwapChainSupportDetails;

	class App final {
	public:
		RENDER_API void Run();

	private:
		void Init();
		void MainLoop();
		void Cleanup();
		void CreateInstance();

		void InitObjects();

		void SetupDebugMsgr();
		void PopulateDebugMsgrCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		
		std::vector<const char*> GetReqExtensions();

		bool CheckValLayerSupport();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
			, VkDebugUtilsMessageTypeFlagsEXT messageType
			, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
			, void* pUserData
		);

		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		QFamilyInd FindQFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();
		void CreateSurface();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateVertexBuffers();
		void CreateSyncObjects();

		void CreateBuffer(
			  VkDevice& device
			, VkDeviceSize size
			, VkBufferUsageFlags usgFlags
			, VkMemoryPropertyFlags props
			, VkBuffer& buffer
			, VkDeviceMemory& bufferMem
		);

		void CopyBuffer(
			  VkDevice& device
			, VkCommandPool& commandPool
			, VkQueue& graphicsQueue
			, VkBuffer fromBuffer
			, VkBuffer toBuffer
			, VkDeviceSize size
		);

		void CleanupSwapChain();
		void RecreateSwapChain();

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);

		void DrawFrame();

		uint32_t FindMemType(uint32_t memTypeFilter, VkMemoryPropertyFlags properties);

		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		bool CheckDeviceExtSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	private_var:
		static const uint8_t s_MaxFramesInFlight = 2;
		uint32_t m_CurrentFrame = 0;

		Window* m_PWindow;

		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		VkSurfaceKHR m_Surface;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_Framebuffers;

		VkRenderPass m_RenderPass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMem;

		VkCommandPool m_ComandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_IFFences;

		VkQueue m_GraphicsQueue;
		VkQueue	m_PresentQueue;
	};

}