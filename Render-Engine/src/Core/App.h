#pragma once

// Core
#include <Defs.h>
#include <Core/Graphics/Vertex.h>
//

// Front core
#include <Rnd/ObjectSettings.h>
//

// Vulkan
#include <vulkan/vulkan_core.h>
//

// STL
#include <vector>
#include <functional>
#include <unordered_map>
//

namespace Render {

	// Forward declared local stuff

	class Window;
	class GDevice;
	class GModel;
  
	class GObject;
	class GCamera;
	class GCameraController;

	struct QFamilyInd;
	struct SwapChainSupportDetails;

	class App final {
	public:
		void Run(std::function<void()> start, std::function<void()> update, std::function<std::unordered_map<uint64_t, rnd::ObjectSettings*>()> drawList);

	private:
		void Init();
		void InitGUI();
		void MainLoop(std::function<void()> start, std::function<void()> update, std::function<std::unordered_map<uint64_t, rnd::ObjectSettings*>()> drawList);
		void Cleanup();

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
		void CreateCommandBuffers();

		void CreateDepthRes();

		[[nodiscard]] VkFormat GetSupportedDepthFormat();

		[[nodiscard]] bool HasStencil(VkFormat format);

		void CreateTextureImage();
		void CreateTextureImageView();
		void CreateTextureSampler();

		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();
		void CreateSyncObjects();

		void CreateComputePipeline();
		void CreateStorageBuffers();

		void CreateComputeDescriptorSetsLayout();
		void CreateComputeDescriptorSets();

		void CreateComputeCommandBuffers();

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

		void RecComputeCommandBuffer(VkCommandBuffer commandBuffer);


		void DrawFrame();
		void UpdateUniformBuffer(uint32_t currentFrame);

		[[nodiscard]] VkShaderModule CreateShaderModule(const std::vector<char>& code);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		void ClearVertices();

		// Draw
		void DrawObjects(VkCommandBuffer& commBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descSet);

	private_var:
		static const uint8_t s_MaxFramesInFlight = 2;
		static const bool s_VSync = false;

		static VkClearColorValue s_BgColor;

		uint32_t m_CurrentFrame = 0;

		GDevice* m_Device;
		GCamera* m_Camera;
		GCameraController* m_CameraController;

		//VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

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

		VkDescriptorSetLayout m_ComputeDescSetLayout;
		VkPipelineLayout m_ComputePipelineLayout;
		VkPipeline m_ComputePipeline;

		std::vector<VkDescriptorSet> m_ComputeDescSets;
		std::vector<VkCommandBuffer> m_ComputeCommandBuffers;

		std::vector<VkSemaphore> m_ComputeFinishedSemaphores;
		std::vector<VkFence> m_ComputeIFFences;

		VkImage m_TextureImg;
		VkDeviceMemory m_TextureImgMem;
		VkImageView m_TextureImgView;
		VkSampler m_TextureSampler;

		VkImage m_DepthImg;
		VkDeviceMemory m_DepthImgMem;
		VkImageView m_DepthImgView;

		std::vector<VkBuffer> m_UniformBuffers;
		std::vector<VkDeviceMemory> m_UniformBuffersMem;

		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_IFFences;

		// Vertices
		VkBuffer m_VertexBuffer;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_VertexBufferMem;
		VkDeviceMemory m_IndexBufferMem;

		std::vector<VkBuffer> m_StorageBuffers;
		std::vector<VkDeviceMemory> m_StorageBuffersMem;

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		// Draw & Models
		std::unordered_map<uint64_t, GObject*> m_Objects;


		// TEMP
		uint32_t s_ParticleCount = 1000;
	};

}