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
		QFamilyInd GetQFamilies(VkPhysicalDevice device);

		void CreateLogicalDevice();
		void CreateSurface();
		void CreateSwapChain();

		[[nodiscard]] VkImageView CreateImageView(
			  VkImage img
			, VkFormat format
			, VkImageAspectFlags aspFlags
		);

		void CreateImageViews();
		
		void CreateRenderPass();
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline();
		void CreateFrameBuffers();
		void CreateCommandPool();
		void CreateCommandBuffers();

		void CreateDepthRes();

		[[nodiscard]] VkFormat GetSupportedDepthFormat();
		
		[[nodiscard]] VkFormat GetSupportedFormat(
			  const std::vector<VkFormat>& formats
			, VkImageTiling tiling
			, VkFormatFeatureFlags features
		);

		[[nodiscard]] bool HasStencil(VkFormat format);

		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();

		void CreateVertexBuffers();
		void CreateIndexBuffer();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateSyncObjects();

		void CreateImage(
			  uint32_t width
			, uint32_t height
			, VkFormat format
			, VkImageTiling tiling
			, VkImageUsageFlags usgFlags
			, VkMemoryPropertyFlags props
			, VkImage& img
			, VkDeviceMemory& imgMem
		);

		void TransitionImgLayout(
			  VkImage img, VkFormat format
			, VkImageLayout oldLayout, VkImageLayout newLayout
		);

		[[nodiscard]] VkCommandBuffer BeginSTCommands();
		void EndSTCommands(VkCommandBuffer cmdBuffer);

		void CreateBuffer(
			  VkDevice& device
			, VkDeviceSize size
			, VkBufferUsageFlags usgFlags
			, VkMemoryPropertyFlags props
			, VkBuffer& buffer
			, VkDeviceMemory& bufferMem
		);

		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImg(VkBuffer buffer, VkImage img, uint32_t width, uint32_t height);

		void CleanupSwapChain();
		void RecreateSwapChain();

		void RecCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index);

		void DrawFrame();
		void UpdateUniformBuffer(uint32_t currentFrame);

		uint32_t GetMemType(uint32_t memTypeFilter, VkMemoryPropertyFlags properties);

		[[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char>& code);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		bool CheckDeviceExtSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	private_var:
		static const uint8_t s_MaxFramesInFlight = 8;
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

		std::vector<VkImage> m_SwapChainImgs;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_Framebuffers;

		VkRenderPass m_RenderPass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		VkDescriptorSetLayout m_DescSetLayout;
		VkDescriptorPool m_DescPool;

		std::vector<VkDescriptorSet> m_DescSets;

		VkBuffer m_VertexBuffer;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_VertexBufferMem;
		VkDeviceMemory m_IndexBufferMem;

		VkImage m_TextureImg;
		VkDeviceMemory m_TextureImgMem;
		VkImageView m_TextureImgView;
		VkSampler m_TextureSampler;

		VkImage m_DepthImg;
		VkDeviceMemory m_DepthImgMem;
		VkImageView m_DepthImgView;

		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMem;

		VkCommandPool m_ComandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_IFFences;

		VkQueue m_GraphicsQueue;
		VkQueue	m_PresentQueue;
	};

}