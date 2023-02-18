#include "pch.h"

// Core
#include "App.h"
#include "Extra.h"
#include "Helper.h"
#include <Core.h>
#include <Core/Time.h>
#include <Window/Window.h>
//

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//

// TOL
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
//

// Graphcis & Display
#include "Graphics/Vertex.h"
#include "Graphics/UniformObject.h"
#include "Display/GDevice.h"
#include "Display/GBuffer.h"
#include "Display/GModel.h"
#include "Display/GObject.h"
#include "Display/GCamera.h"
//

// UI
#include "UI/UI.h"
//

// Controller
#include "Controller/GCameraController.h"
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

	// Static inits
	VkClearColorValue App::s_BgColor = VkClearColorValue{ 0.07f, 0.07f, 0.07f, 1.0f };

	//

	void App::Run() {
		Init();
		InitGUI();
		MainLoop();
		Cleanup();
	}

	void App::Init() {
		Window* pWindow = new Window{ 1280, 720, "Fluid Sim" };
		m_Device = new GDevice{ *pWindow };
		m_Camera = new GCamera{};
		m_CameraController = new GCameraController(pWindow, m_Camera);
		
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();

		CreateDepthRes();
		CreateFrameBuffers();

		CreateTextureImage();
		CreateTextureImageView();
		CreateTextureSampler();

		CreateUniformBuffers();
		CreateCommandBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateSyncObjects();

		// For now objects added in a hacky way
		m_Objects.push_back(new GObject{ *m_Device, "models/room.obj" });
		m_Objects.push_back(new GObject{ *m_Device, "models/room.obj" });
		m_Objects[0]->tranform.translate.x += 2;

	}

	void App::InitGUI() {
		UI::Begin(m_Device, m_VkInstance, m_RenderPass, m_DescPool, s_MaxFramesInFlight);
	}

	void App::MainLoop() {
		
		float modi = 1.0f;
		static auto startTime = std::chrono::high_resolution_clock::now();
		while (!m_Device->GetWindow()->ShouldClose()) {
			auto		currentTime = std::chrono::high_resolution_clock::now();
			float		deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			startTime = currentTime;

			Time::_SetDeltaTime(deltaTime);

			DrawFrame();
			glfwPollEvents();

			m_CameraController->Update();			

			m_Camera->SetPerspProjection(glm::radians(70.0f), static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height), 0.1f, 1000.0f);
			//m_Camera->SetViewTrg(glm::vec3{ 3.0f, 6.0f + modi, 10.0f }, glm::vec3{ 0.0f });

			//m_Camera->SetViewTrg(glm::vec3{ UI::cameraPos[0], UI::cameraPos[1], UI::cameraPos[2] }, glm::vec3{1.0f});


			std::cout << "FPS: " <<  static_cast<int>(1.0f / deltaTime) << "\n";
			modi += 3.0f * deltaTime;
		}

		vkDeviceWaitIdle(m_Device->GetDevice());
	}

	void App::Cleanup() {
		UI::End();

		CleanupSwapChain();
		
		vkDestroySampler(m_Device->GetDevice(), m_TextureSampler, nullptr);
		vkDestroyImageView(m_Device->GetDevice(), m_TextureImgView, nullptr);

		vkDestroyImage(m_Device->GetDevice(), m_TextureImg, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_TextureImgMem, nullptr);

		for (size_t i = 0; i < s_MaxFramesInFlight; i++) {
			vkDestroyBuffer(m_Device->GetDevice(), m_UniformBuffers[i], nullptr);
			vkFreeMemory(m_Device->GetDevice(), m_UniformBuffersMem[i], nullptr);
		}

		vkDestroyDescriptorSetLayout(m_Device->GetDevice(), m_DescSetLayout, nullptr);

		vkDestroyBuffer(m_Device->GetDevice(), m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_IndexBufferMem, nullptr);

		vkDestroyBuffer(m_Device->GetDevice(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_VertexBufferMem, nullptr);

		for (size_t i = 0; i < s_MaxFramesInFlight; i++) {
			vkDestroySemaphore(m_Device->GetDevice(), m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device->GetDevice(), m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_Device->GetDevice(), m_IFFences[i], nullptr);
		}

		vkDestroyCommandPool(m_Device->GetDevice(), m_Device->GetCommandPool(), nullptr);

		for (auto elem : m_Framebuffers)
			vkDestroyFramebuffer(m_Device->GetDevice(), elem, nullptr);

		vkDestroyPipeline(m_Device->GetDevice(), m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetDevice(), m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_Device->GetDevice(), m_RenderPass, nullptr);

		for (auto elem : m_SwapChainImageViews)
			vkDestroyImageView(m_Device->GetDevice(), elem, nullptr);

		delete m_Device;
		delete m_Camera;
		delete m_CameraController;
	}

	void App::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = m_Device->QuerySwapChainSupport(m_Device->GetPhysicalDevice());

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);

		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Device->GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QFamilyInd ind = m_Device->GetQFamilies(m_Device->GetPhysicalDevice());
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

		if (vkCreateSwapchainKHR(m_Device->GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
			throw std::runtime_error("Failed to create swap chain!");

		vkGetSwapchainImagesKHR(m_Device->GetDevice(), m_SwapChain, &imageCount, nullptr);
		m_SwapChainImgs.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetDevice(), m_SwapChain, &imageCount, m_SwapChainImgs.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		
		m_SwapChainExtent = extent;
	}

	[[nodiscard]] VkImageView App::CreateImageView(VkImage img, VkFormat format, VkImageAspectFlags aspFlags) {
		VkImageViewCreateInfo imgViewInfo{};
		imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewInfo.image = img;
		imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imgViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;

		imgViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imgViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		imgViewInfo.subresourceRange.aspectMask = aspFlags;
		imgViewInfo.subresourceRange.baseMipLevel = 0;
		imgViewInfo.subresourceRange.levelCount = 1;
		imgViewInfo.subresourceRange.baseArrayLayer = 0;
		imgViewInfo.subresourceRange.layerCount = 1;

		VkImageView imgView;
		if (vkCreateImageView(m_Device->GetDevice(), &imgViewInfo, nullptr, &imgView) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image view!");

		return imgView;
	}

	void App::CreateImageViews() {
		m_SwapChainImageViews.resize(m_SwapChainImgs.size());

		for (size_t i = 0; i < m_SwapChainImgs.size(); i++)
			m_SwapChainImageViews[i] = CreateImageView(m_SwapChainImgs[i], m_SwapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void App::CreateRenderPass() {
		VkAttachmentDescription colorAttach{};
		colorAttach.format				= m_SwapChainImageFormat;
		colorAttach.samples				= VK_SAMPLE_COUNT_1_BIT;

		// The data in the attachment will be cleared before a render pass and store after
		colorAttach.loadOp				= VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttach.storeOp				= VK_ATTACHMENT_STORE_OP_STORE;
		
		// Image layout is initialized as undefined because we don't care about
		// the previous layout because the image is cleared anyway
		colorAttach.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttach.finalLayout			= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachRef{};
		colorAttachRef.attachment		= 0;
		colorAttachRef.layout			= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttach{};
		depthAttach.format				= GetSupportedDepthFormat();
		depthAttach.samples				= VK_SAMPLE_COUNT_1_BIT;
		depthAttach.loadOp				= VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttach.storeOp				= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttach.stencilLoadOp		= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttach.stencilStoreOp		= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttach.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttach.finalLayout			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachRef{};
		depthAttachRef.attachment		= 1;
		depthAttachRef.layout			= VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDependency dep{};
		dep.srcSubpass					= VK_SUBPASS_EXTERNAL;
		dep.dstSubpass					= 0;
		dep.srcStageMask				= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dep.srcAccessMask				= 0;
		dep.dstStageMask				= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dep.dstAccessMask				= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint		= VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount	= 1;
		subpass.pColorAttachments		= &colorAttachRef;
		subpass.pDepthStencilAttachment = &depthAttachRef;

		std::array<VkAttachmentDescription, 2> attachs = { colorAttach, depthAttach };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType			= VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount	= static_cast<uint32_t>(attachs.size());
		renderPassInfo.pAttachments		= attachs.data();
		renderPassInfo.subpassCount		= 1;
		renderPassInfo.pSubpasses		= &subpass;
		renderPassInfo.dependencyCount	= 1;
		renderPassInfo.pDependencies	= &dep;

		if (vkCreateRenderPass(m_Device->GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
			throw std::runtime_error("Failed to create render pass!");
	}

	void App::CreateDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uniBuffLayoutBinding{};
		uniBuffLayoutBinding.binding = 0;
		uniBuffLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniBuffLayoutBinding.descriptorCount = 1;
		uniBuffLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uniBuffLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uniBuffLayoutBinding, samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device->GetDevice(), &layoutInfo, nullptr, &m_DescSetLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout!");
	}

	void App::CreateGraphicsPipeline() {
		auto vertShaderCode = Helper::ReadFile("shaders/vert.spv");
		auto fragShaderCode = Helper::ReadFile("shaders/frag.spv");

		VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);
		
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages {
			  vertShaderStageInfo
			, fragShaderStageInfo
		};

		auto bindingDescr = Vertex::GetBindingDescription();
		auto attributeDescrs = Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescrs.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescr;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescrs.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
		inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportStateInfo{};
		viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateInfo.viewportCount = 1;
		viewportStateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
		rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerInfo.depthClampEnable = VK_FALSE;
		rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerInfo.lineWidth = 1.0f;
		rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizerInfo.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
		multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingInfo.sampleShadingEnable = VK_FALSE;
		multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlend{};
		colorBlend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlend.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendingInfo{};
		colorBlendingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingInfo.logicOpEnable = VK_FALSE;
		colorBlendingInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendingInfo.attachmentCount = 1;
		colorBlendingInfo.pAttachments = &colorBlend;
		colorBlendingInfo.blendConstants[0] = 0.0f;
		colorBlendingInfo.blendConstants[1] = 0.0f;
		colorBlendingInfo.blendConstants[2] = 0.0f;
		colorBlendingInfo.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dStates = {
			  VK_DYNAMIC_STATE_VIEWPORT
			, VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dStates.size());
		dynamicStateInfo.pDynamicStates = dStates.data();
		
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;

		if (vkCreatePipelineLayout(m_Device->GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportStateInfo;
		pipelineInfo.pRasterizationState = &rasterizerInfo;
		pipelineInfo.pMultisampleState = &multisamplingInfo;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlendingInfo;
		pipelineInfo.pDynamicState = &dynamicStateInfo;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = m_RenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		if (vkCreateGraphicsPipelines(m_Device->GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");

		vkDestroyShaderModule(m_Device->GetDevice(), vertShaderModule, nullptr);
		vkDestroyShaderModule(m_Device->GetDevice(), fragShaderModule, nullptr);
	}

	void App::CreateFrameBuffers() {
		m_Framebuffers.resize(m_SwapChainImageViews.size());
		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
			std::array<VkImageView, 2> attachs = { m_SwapChainImageViews[i], m_DepthImgView };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachs.size());
			framebufferInfo.pAttachments = attachs.data();
			framebufferInfo.width = m_SwapChainExtent.width;
			framebufferInfo.height = m_SwapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device->GetDevice(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!");
		}
	}

	void App::CreateCommandBuffers() {
		m_CommandBuffers.resize(s_MaxFramesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocInfo{};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandPool = m_Device->GetCommandPool();
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device->GetDevice(), &commandBufferAllocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers!");
	}

	void App::CreateDepthRes() {
		VkFormat depthForm = GetSupportedDepthFormat();

		CreateImage(
			  m_SwapChainExtent.width
			, m_SwapChainExtent.height
			, depthForm
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_DepthImg
			, m_DepthImgMem
		);

		m_DepthImgView = CreateImageView(m_DepthImg, depthForm, VK_IMAGE_ASPECT_DEPTH_BIT);

		TransitionImgLayout(
			  m_DepthImg, depthForm
			, VK_IMAGE_LAYOUT_UNDEFINED
			, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		);
	}

	[[nodiscard]] VkFormat App::GetSupportedDepthFormat() {
		return m_Device->GetSupportedFormat(
			  { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }
			, VK_IMAGE_TILING_OPTIMAL
			, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	[[nodiscard]] bool App::HasStencil(VkFormat format) {
		// Revisit
		return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
	}

	void App::CreateTextureImage() {
		int width, height, channels;
		stbi_uc* pixels = stbi_load("textures/room.png", &width, &height, &channels, STBI_rgb_alpha);
		
		VkDeviceSize imgSize = (uint64_t)width * height * 4;

		if (!pixels)
			throw std::runtime_error("Failed to load texture image!");

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		GBuffer::CreateBuffer(
			  m_Device
			, imgSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, stagingBuffer
			, stagingBufferMem
		);

		void* data;
		vkMapMemory(m_Device->GetDevice(), stagingBufferMem, 0, imgSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imgSize));
		vkUnmapMemory(m_Device->GetDevice(), stagingBufferMem);

		stbi_image_free(pixels);
		CreateImage(
			  width
			, height
			, VK_FORMAT_R8G8B8A8_SRGB
			, VK_IMAGE_TILING_OPTIMAL
			, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_TextureImg
			, m_TextureImgMem
		);

		TransitionImgLayout(
			  m_TextureImg, VK_FORMAT_R8G8B8A8_SRGB
			, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL	
		);

		CopyBufferToImg(
			  stagingBuffer, m_TextureImg
			, static_cast<uint32_t>(width), static_cast<uint32_t>(height)
		);

		TransitionImgLayout(
			m_TextureImg, VK_FORMAT_R8G8B8A8_SRGB
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
		);

		vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), stagingBufferMem, nullptr);
	}

	void App::CreateTextureImageView() {
		m_TextureImgView = CreateImageView(m_TextureImg, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void App::CreateTextureSampler() {
		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_FALSE;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(m_Device->GetPhysicalDevice(), &props);

		samplerCreateInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Device->GetDevice(), &samplerCreateInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture sampler!");
	}

	void App::CreateImage(
		  uint32_t width
		, uint32_t height
		, VkFormat format
		, VkImageTiling tiling
		, VkImageUsageFlags usgFlags
		, VkMemoryPropertyFlags props
		, VkImage& img
		, VkDeviceMemory& imgMem) {

		VkImageCreateInfo imgInfo{};
		imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imgInfo.imageType = VK_IMAGE_TYPE_2D;
		imgInfo.extent.width = width;
		imgInfo.extent.height = height;
		imgInfo.mipLevels = 1;
		imgInfo.arrayLayers = 1;
		imgInfo.format = format;
		imgInfo.tiling = tiling;
		imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imgInfo.usage = usgFlags;
		imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imgInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(m_Device->GetDevice(), &imgInfo, nullptr, &img) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image!");

		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(m_Device->GetDevice(), img, &memReq);

		VkMemoryAllocateInfo mallocInfo{};
		mallocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mallocInfo.allocationSize = memReq.size;
		mallocInfo.memoryTypeIndex = m_Device->GetMemType(memReq.memoryTypeBits, props);

		if (vkAllocateMemory(m_Device->GetDevice(), &mallocInfo, nullptr, &imgMem) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image memory!");

		vkBindImageMemory(m_Device->GetDevice(), img, imgMem, 0);
	}

	void App::TransitionImgLayout(
		  VkImage img, VkFormat format, VkImageLayout oldLayout
		, VkImageLayout newLayout) {

		VkCommandBuffer cmdBuffer = m_Device->BeginSTCommands();

		VkImageMemoryBarrier memBarrier{};
		memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		memBarrier.oldLayout = oldLayout;
		memBarrier.newLayout = newLayout;
		memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		memBarrier.image = img;
		memBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		memBarrier.subresourceRange.baseMipLevel = 0;
		memBarrier.subresourceRange.levelCount = 1;
		memBarrier.subresourceRange.baseArrayLayer = 0;
		memBarrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags srcStageFlags, dstStageFlags;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			memBarrier.srcAccessMask = 0;
			memBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			srcStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dstStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			memBarrier.srcAccessMask = 0;
			memBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dstStageFlags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else {
			throw std::invalid_argument("Unsupported layout transition!");
		}

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			memBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (HasStencil(format))
				memBarrier.subresourceRange.aspectMask = memBarrier.subresourceRange.aspectMask | VK_IMAGE_ASPECT_STENCIL_BIT;
		}
		else {
			memBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		vkCmdPipelineBarrier(
			  cmdBuffer
			, srcStageFlags
			, dstStageFlags
			, 0, 0
			, nullptr, 0
			, nullptr, 1
			, &memBarrier
		);

		m_Device->EndSTCommands(cmdBuffer);
	}

	void App::CreateUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	
		m_UniformBuffers.resize(s_MaxFramesInFlight);
		m_UniformBuffersMem.resize(s_MaxFramesInFlight);

		for (size_t i = 0; i < s_MaxFramesInFlight; i++)
			GBuffer::CreateBuffer(
				  m_Device
				, bufferSize
				, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				, m_UniformBuffers[i]
				, m_UniformBuffersMem[i]
			);
	}

	void App::CopyBufferToImg(VkBuffer buffer, VkImage img, uint32_t width, uint32_t height) {
		VkCommandBuffer cmdBuffer = m_Device->BeginSTCommands();

		VkBufferImageCopy imgRegion{};
		imgRegion.bufferOffset = 0;
		imgRegion.bufferRowLength = 0;
		imgRegion.bufferImageHeight = 0;

		imgRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imgRegion.imageSubresource.mipLevel = 0;
		imgRegion.imageSubresource.baseArrayLayer = 0;
		imgRegion.imageSubresource.layerCount = 1;
		
		imgRegion.imageOffset = { 0, 0, 0 };
		imgRegion.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(
			  cmdBuffer, buffer, img
			, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			, 1, &imgRegion
		);

		m_Device->EndSTCommands(cmdBuffer);
	}

	void App::CreateDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> descPoolSizes{};
		descPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descPoolSizes[0].descriptorCount = static_cast<uint32_t>(s_MaxFramesInFlight);
		descPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descPoolSizes[1].descriptorCount = static_cast<uint32_t>(s_MaxFramesInFlight) * 2;

		VkDescriptorPoolCreateInfo descPoolInfo{};
		descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descPoolInfo.poolSizeCount = static_cast<uint32_t>(descPoolSizes.size());
		descPoolInfo.pPoolSizes = descPoolSizes.data();
		descPoolInfo.maxSets = static_cast<uint32_t>(s_MaxFramesInFlight) * 2;

		if (vkCreateDescriptorPool(m_Device->GetDevice(), &descPoolInfo, nullptr, &m_DescPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor pool!");
	}

	void App::CreateDescriptorSets() {
		std::vector<VkDescriptorSetLayout> layouts(s_MaxFramesInFlight, m_DescSetLayout);

		VkDescriptorSetAllocateInfo descAllocInfo{};
		descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descAllocInfo.descriptorPool = m_DescPool;
		descAllocInfo.descriptorSetCount = static_cast<uint32_t>(s_MaxFramesInFlight);
		descAllocInfo.pSetLayouts = layouts.data();

		m_DescSets.resize(s_MaxFramesInFlight);
		if (vkAllocateDescriptorSets(m_Device->GetDevice(), &descAllocInfo, m_DescSets.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate descriptor sets!");

		for (size_t i = 0; i < s_MaxFramesInFlight; i++) {
			VkDescriptorBufferInfo descBufferInfo{};
			descBufferInfo.buffer = m_UniformBuffers[i];
			descBufferInfo.offset = 0;
			descBufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imgInfo{};
			imgInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imgInfo.imageView = m_TextureImgView;
			imgInfo.sampler = m_TextureSampler;

			std::array<VkWriteDescriptorSet, 2> descWrites{};
			descWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrites[0].dstSet = m_DescSets[i];
			descWrites[0].dstBinding = 0;
			descWrites[0].dstArrayElement = 0;
			descWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descWrites[0].descriptorCount = 1;
			descWrites[0].pBufferInfo = &descBufferInfo;

			descWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descWrites[1].dstSet = m_DescSets[i];
			descWrites[1].dstBinding = 1;
			descWrites[1].dstArrayElement = 0;
			descWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descWrites[1].descriptorCount = 1;
			descWrites[1].pImageInfo = &imgInfo;

			vkUpdateDescriptorSets(m_Device->GetDevice(), static_cast<uint32_t>(descWrites.size()), descWrites.data(), 0, nullptr);
		}
	}

	void App::CreateSyncObjects() {
		m_ImageAvailableSemaphores.resize(s_MaxFramesInFlight);
		m_RenderFinishedSemaphores.resize(s_MaxFramesInFlight);
		m_IFFences.resize(s_MaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < s_MaxFramesInFlight; i++)
			if (vkCreateSemaphore(m_Device->GetDevice(), &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS
				|| vkCreateSemaphore(m_Device->GetDevice(), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS
				|| vkCreateFence(m_Device->GetDevice(), &fenceInfo, nullptr, &m_IFFences[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create sync objects!");
	}

	void App::CleanupSwapChain() {
		vkDestroyImageView(m_Device->GetDevice(), m_DepthImgView, nullptr);
		vkDestroyImage(m_Device->GetDevice(), m_DepthImg, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_DepthImgMem, nullptr);

		for (size_t i = 0; i < m_Framebuffers.size(); i++)
			vkDestroyFramebuffer(m_Device->GetDevice(), m_Framebuffers[i], nullptr);

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
			vkDestroyImageView(m_Device->GetDevice(), m_SwapChainImageViews[i], nullptr);

		vkDestroySwapchainKHR(m_Device->GetDevice(), m_SwapChain, nullptr);
	}

	void App::RecreateSwapChain() {
		int width = 0, height = 0;

		while (!width && !height) {
			glfwGetFramebufferSize(m_Device->GetWindow()->GetGLFWwindowPointer(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device->GetDevice());
		
		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateDepthRes();
		CreateFrameBuffers();
	}

	void App::RecCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index) {
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!");

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_RenderPass;
		renderPassBeginInfo.framebuffer = m_Framebuffers[index];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 2> clearVals{};
		clearVals[0].color = s_BgColor;
		clearVals[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearVals.size());
		renderPassBeginInfo.pClearValues = clearVals.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
		
		DrawObjects(commandBuffer, m_PipelineLayout, m_DescSets[m_CurrentFrame]);

		VkViewport viewPort{};
		viewPort.x = 0.0f;
		viewPort.y = 0.0f;
		viewPort.width = static_cast<float>(m_SwapChainExtent.width);
		viewPort.height = static_cast<float>(m_SwapChainExtent.height);
		viewPort.minDepth = 0.0f;
		viewPort.maxDepth = 1.0f;

		vkCmdSetViewport(commandBuffer, 0, 1, &viewPort);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_SwapChainExtent;

		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		
		UI::Main(commandBuffer);
		
		vkCmdEndRenderPass(commandBuffer);


		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}

	void App::DrawFrame() {
		vkWaitForFences(m_Device->GetDevice(), 1, &m_IFFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imgInd;
		VkResult res = vkAcquireNextImageKHR(m_Device->GetDevice(), m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imgInd);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (res != VK_SUCCESS)
			throw::std::runtime_error("Failed to acquire swap chain image");

		UpdateUniformBuffer(m_CurrentFrame);
		RecCommandBuffer(m_CommandBuffers[m_CurrentFrame], imgInd);

		vkResetFences(m_Device->GetDevice(), 1, &m_IFFences[m_CurrentFrame]);
		//vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_IFFences[m_CurrentFrame]) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer!");

		std::array<VkSwapchainKHR, 1> swapChains = { m_SwapChain };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = static_cast<uint32_t>(swapChains.size());
		presentInfo.pSwapchains = swapChains.data();
		presentInfo.pImageIndices = &imgInd;

		res = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || m_Device->GetWindow()->framebufferResized) {
			m_Device->GetWindow()->framebufferResized = false;
			RecreateSwapChain();
		}
		else if (res != VK_SUCCESS)
			throw std::runtime_error("Failed to present swap chain image!");

		m_CurrentFrame = (m_CurrentFrame + 1) % s_MaxFramesInFlight;
	}

	void App::UpdateUniformBuffer(uint32_t currentFrame) {
		UniformBufferObject uniBuff{};
		uniBuff.model = m_Camera->GetInvViewMat();
		uniBuff.view = m_Camera->GetViewMat();
		uniBuff.proj = m_Camera->GetProjMat();

		void* data;
		vkMapMemory(m_Device->GetDevice(), m_UniformBuffersMem[currentFrame], 0, sizeof(uniBuff), 0, &data);
		memcpy(data, &uniBuff, sizeof(uniBuff));
		vkUnmapMemory(m_Device->GetDevice(), m_UniformBuffersMem[currentFrame]);
	}

	[[nodiscard]] VkShaderModule App::CreateShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_Device->GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");

		return shaderModule;
	}

	VkSurfaceFormatKHR App::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& elem : availableFormats)
			if (elem.format == VK_FORMAT_R8G8B8A8_UNORM && elem.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return elem;

		return availableFormats[0];
	}

	VkPresentModeKHR App::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& elem : availablePresentModes) {
			if (elem == VK_PRESENT_MODE_MAILBOX_KHR && s_VSync)
				return elem;

			if (elem == VK_PRESENT_MODE_IMMEDIATE_KHR && !s_VSync)
				return elem;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D App::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		
		int width, height;
		glfwGetFramebufferSize(m_Device->GetWindow()->GetGLFWwindowPointer(), &width, &height);

		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
		return actualExtent;
	}

	void App::ClearVertices() {
		m_Vertices.clear();
		m_Indices.clear();
	}

	void App::DrawObjects(VkCommandBuffer& commBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descSet) {
		
		PushConstantData push{};

		vkCmdBindDescriptorSets(commBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descSet, 0, nullptr);

		for (auto& elem : m_Objects) {
			if (!elem)
				continue;

			push.modelMatrix = elem->tranform.Model();
			push.normalMatrix = elem->tranform.Normal();

			vkCmdPushConstants(
				  commBuffer
				, pipelineLayout
				, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0
				, sizeof(PushConstantData)
				, &push
			);

			elem->m_Model->Bind(commBuffer);
			elem->m_Model->Draw(commBuffer);
		}
	}
}