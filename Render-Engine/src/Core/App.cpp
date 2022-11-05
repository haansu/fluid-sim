#include "pch.h"

#include "App.h"
#include "Extra.h"
#include "Helper.h"
#include "Graphics/Vertex.h"
#include "Graphics/GraphicsObj.h"

// GLM
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//

#include "../Core.h"
#include "../Window/Window.h"

#include <algorithm>
#include <array>
#include <limits>
#include <optional>
#include <set>
#include <vector>
#include <chrono>

namespace Render {

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

	void App::Run() {
		Init();
		MainLoop();
		Cleanup();
	}

	void App::Init() {
		InitObjects();
		CreateInstance();
		
		SetupDebugMsgr();

		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateCommandPool();
		
		CreateDepthRes();
		CreateFrameBuffers();

		CreateTextureImage();
		CreateTextureImageView();
		CreateTextureSampler();
		
		CreateVertexBuffers();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateCommandBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateSyncObjects();
	}

	void App::InitObjects() {
		m_PWindow = new Window{ 800, 600, "Vulkan" };
	}

	void App::MainLoop() {
		while (!m_PWindow->ShouldClose()) {
			DrawFrame();
			glfwPollEvents();
		}

		vkDeviceWaitIdle(m_Device);
	}

	void App::Cleanup() {
		CleanupSwapChain();
		
		vkDestroySampler(m_Device, m_TextureSampler, nullptr);
		vkDestroyImageView(m_Device, m_TextureImgView, nullptr);

		vkDestroyImage(m_Device, m_TextureImg, nullptr);
		vkFreeMemory(m_Device, m_TextureImgMem, nullptr);

		for (size_t i = 0; i < s_MaxFramesInFlight; i++) {
			vkDestroyBuffer(m_Device, m_UniformBuffers[i], nullptr);
			vkFreeMemory(m_Device, m_UniformBuffersMem[i], nullptr);
		}

		vkDestroyDescriptorSetLayout(m_Device, m_DescSetLayout, nullptr);

		vkDestroyBuffer(m_Device, m_IndexBuffer, nullptr);
		vkFreeMemory(m_Device, m_IndexBufferMem, nullptr);

		vkDestroyBuffer(m_Device, m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device, m_VertexBufferMem, nullptr);

		for (size_t i = 0; i < s_MaxFramesInFlight; i++) {
			vkDestroySemaphore(m_Device, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_Device, m_IFFences[i], nullptr);
		}

		vkDestroyCommandPool(m_Device, m_ComandPool, nullptr);

		for (auto elem : m_Framebuffers)
			vkDestroyFramebuffer(m_Device, elem, nullptr);

		vkDestroyPipeline(m_Device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);

		for (auto elem : m_SwapChainImageViews)
			vkDestroyImageView(m_Device, elem, nullptr);

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
		vkDestroyDevice(m_Device, nullptr);
		
		if (enableValLayers)
			DestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugMessenger, nullptr);

		vkDestroySurfaceKHR(m_VkInstance, m_Surface, nullptr);
		vkDestroyInstance(m_VkInstance, nullptr);

		delete m_PWindow;
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
			throw std::runtime_error("Failed to create instance!");
		
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

	VKAPI_ATTR VkBool32 VKAPI_CALL App::DebugCallback(
		  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity
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

	bool App::IsDeviceSuitable(VkPhysicalDevice device) {
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

	QFamilyInd App::GetQFamilies(VkPhysicalDevice device) {
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
		} else
			createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device!");

		vkGetDeviceQueue(m_Device, ind.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, ind.presentFamily.value(), 0, &m_PresentQueue);
	}

	void App::CreateSurface() {
		if (glfwCreateWindowSurface(m_VkInstance, m_PWindow->GetGLFWwindowPointer(), nullptr, &m_Surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");
	}

	void App::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_PhysicalDevice);

		VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		
		// NOTE: LOOK HERE FOR 3D LATER
		VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
			imageCount = swapChainSupport.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QFamilyInd ind = GetQFamilies(m_PhysicalDevice);
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

	[[nodiscard]] VkImageView App::CreateImageView(VkImage img, VkFormat format, VkImageAspectFlags aspFlags) {
		VkImageViewCreateInfo imgViewInfo{};
		imgViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imgViewInfo.image = img;
		imgViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imgViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;

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
		if (vkCreateImageView(m_Device, &imgViewInfo, nullptr, &imgView) != VK_SUCCESS)
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

		if (vkCreateRenderPass(m_Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
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

		if (vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescSetLayout) != VK_SUCCESS)
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

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_DescSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

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

		if (vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
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

		if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");

		vkDestroyShaderModule(m_Device, vertShaderModule, nullptr);
		vkDestroyShaderModule(m_Device, fragShaderModule, nullptr);
	}

	void App::CreateFrameBuffers() {
		m_Framebuffers.resize(m_SwapChainImageViews.size());
		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
			VkImageView attachments[] = { m_SwapChainImageViews[i], m_DepthImgView };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = 2;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = m_SwapChainExtent.width;
			framebufferInfo.height = m_SwapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create framebuffer!");
		}
	}

	void App::CreateCommandPool() {
		QFamilyInd qFamInds = GetQFamilies(m_PhysicalDevice);

		VkCommandPoolCreateInfo commandPoolInfo{};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolInfo.queueFamilyIndex = qFamInds.graphicsFamily.value();

		if (vkCreateCommandPool(m_Device, &commandPoolInfo, nullptr, &m_ComandPool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create command pool!");
	}

	void App::CreateCommandBuffers() {
		m_CommandBuffers.resize(s_MaxFramesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocInfo{};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandPool = m_ComandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device, &commandBufferAllocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
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
		return GetSupportedFormat(
			  { VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT }
			, VK_IMAGE_TILING_OPTIMAL
			, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	[[nodiscard]] VkFormat App::GetSupportedFormat(
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

	[[nodiscard]] bool App::HasStencil(VkFormat format) {
		// Revisit
		return format == VK_FORMAT_D24_UNORM_S8_UINT || format == VK_FORMAT_D32_SFLOAT_S8_UINT;
	}

	void App::CreateTextureImage() {
		int width, height, channels;
		stbi_uc* pixels = stbi_load("textures/texture_trees.jpg", &width, &height, &channels, STBI_rgb_alpha);
		
		VkDeviceSize imgSize = (uint64_t)width * height * 4;

		if (!pixels)
			throw std::runtime_error("Failed to load texture image!");

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		CreateBuffer(
			  m_Device
			, imgSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, stagingBuffer
			, stagingBufferMem
		);

		void* data;
		vkMapMemory(m_Device, stagingBufferMem, 0, imgSize, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(imgSize));
		vkUnmapMemory(m_Device, stagingBufferMem);

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

		vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
		vkFreeMemory(m_Device, stagingBufferMem, nullptr);
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
		vkGetPhysicalDeviceProperties(m_PhysicalDevice, &props);

		samplerCreateInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK;
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		if (vkCreateSampler(m_Device, &samplerCreateInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
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

		if (vkCreateImage(m_Device, &imgInfo, nullptr, &img) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image!");

		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(m_Device, img, &memReq);

		VkMemoryAllocateInfo mallocInfo{};
		mallocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mallocInfo.allocationSize = memReq.size;
		mallocInfo.memoryTypeIndex = GetMemType(memReq.memoryTypeBits, props);

		if (vkAllocateMemory(m_Device, &mallocInfo, nullptr, &imgMem) != VK_SUCCESS)
			throw std::runtime_error("Failed to create image memory!");

		vkBindImageMemory(m_Device, img, imgMem, 0);	
	}

	void App::TransitionImgLayout(
		  VkImage img, VkFormat format, VkImageLayout oldLayout
		, VkImageLayout newLayout) {

		VkCommandBuffer cmdBuffer = BeginSTCommands();

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

		EndSTCommands(cmdBuffer);
	}

	[[nodiscard]] VkCommandBuffer App::BeginSTCommands() {
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

	void App::EndSTCommands(VkCommandBuffer cmdBuffer) {
		vkEndCommandBuffer(cmdBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &cmdBuffer;

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueue);

		vkFreeCommandBuffers(m_Device, m_ComandPool, 1, &cmdBuffer);
	}

	void App::CreateVertexBuffers() {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
		
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;

		CreateBuffer(
			  m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, stagingBuffer
			, stagingBufferMem
		);

		void* data;
		vkMapMemory(m_Device, stagingBufferMem, 0, bufferSize, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device, stagingBufferMem);

		CreateBuffer(
			  m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_VertexBuffer
			, m_VertexBufferMem
		);

		CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

		vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
		vkFreeMemory(m_Device, stagingBufferMem, nullptr);
	}

	void App::CreateIndexBuffer() {
		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;
		
		CreateBuffer(
			  m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, stagingBuffer
			, stagingBufferMem
		);

		void* data;
		vkMapMemory(m_Device, stagingBufferMem, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device, stagingBufferMem);

		CreateBuffer(
			  m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_IndexBuffer
			, m_IndexBufferMem
		);

		CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

		vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
		vkFreeMemory(m_Device, stagingBufferMem, nullptr);
	}

	void App::CreateUniformBuffers() {
		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
	
		m_UniformBuffers.resize(s_MaxFramesInFlight);
		m_UniformBuffersMem.resize(s_MaxFramesInFlight);

		for (size_t i = 0; i < s_MaxFramesInFlight; i++)
			CreateBuffer(
				m_Device
				, bufferSize
				, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				, m_UniformBuffers[i]
				, m_UniformBuffersMem[i]
			);
	}

	void App::CreateBuffer(
		  VkDevice& device
		, VkDeviceSize size
		, VkBufferUsageFlags usgFlags
		, VkMemoryPropertyFlags props
		, VkBuffer& buffer
		, VkDeviceMemory& bufferMem) {

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usgFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex buffer!");

		VkMemoryRequirements bufferMemReq{};
		vkGetBufferMemoryRequirements(device, buffer, &bufferMemReq);

		VkMemoryAllocateInfo mallocInfo{};
		mallocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mallocInfo.allocationSize = bufferMemReq.size;
		mallocInfo.memoryTypeIndex = GetMemType(bufferMemReq.memoryTypeBits, props);

		if (vkAllocateMemory(device, &mallocInfo, nullptr, &bufferMem) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate memory for the vertex buffer!");

		vkBindBufferMemory(device, buffer, bufferMem, 0);
	}

	void App::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBuffer cmdBuffer = BeginSTCommands();

		VkBufferCopy bufferCopy{};
		bufferCopy.size = size;
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = 0;
		vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

		EndSTCommands(cmdBuffer);
	}

	void App::CopyBufferToImg(VkBuffer buffer, VkImage img, uint32_t width, uint32_t height) {
		VkCommandBuffer cmdBuffer = BeginSTCommands();

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

		EndSTCommands(cmdBuffer);
	}

	void App::CreateDescriptorPool() {
		std::array<VkDescriptorPoolSize, 2> descPoolSizes{};
		descPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descPoolSizes[0].descriptorCount = static_cast<uint32_t>(s_MaxFramesInFlight);
		descPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descPoolSizes[1].descriptorCount = static_cast<uint32_t>(s_MaxFramesInFlight);

		VkDescriptorPoolCreateInfo descPoolInfo{};
		descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descPoolInfo.poolSizeCount = static_cast<uint32_t>(descPoolSizes.size());
		descPoolInfo.pPoolSizes = descPoolSizes.data();
		descPoolInfo.maxSets = static_cast<uint32_t>(s_MaxFramesInFlight);

		if (vkCreateDescriptorPool(m_Device, &descPoolInfo, nullptr, &m_DescPool) != VK_SUCCESS)
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
		if (vkAllocateDescriptorSets(m_Device, &descAllocInfo, m_DescSets.data()) != VK_SUCCESS)
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

			vkUpdateDescriptorSets(m_Device, static_cast<uint32_t>(descWrites.size()), descWrites.data(), 0, nullptr);
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
			if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS
				|| vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS
				|| vkCreateFence(m_Device, &fenceInfo, nullptr, &m_IFFences[i]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create sync objects!");
	}

	void App::CleanupSwapChain() {
		vkDestroyImageView(m_Device, m_DepthImgView, nullptr);
		vkDestroyImage(m_Device, m_DepthImg, nullptr);
		vkFreeMemory(m_Device, m_DepthImgMem, nullptr);

		for (size_t i = 0; i < m_Framebuffers.size(); i++)
			vkDestroyFramebuffer(m_Device, m_Framebuffers[i], nullptr);

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
			vkDestroyImageView(m_Device, m_SwapChainImageViews[i], nullptr);

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	}

	void App::RecreateSwapChain() {
		int width = 0, height = 0;
		
		while (!width && !height) {
			glfwGetFramebufferSize(m_PWindow->GetGLFWwindowPointer(), &width, &height);
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_Device);
		
		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateDepthRes();
		CreateFrameBuffers();
	}

	void App::RecCommandBuffer(VkCommandBuffer commandBuffer, uint32_t index) {
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to begin recording command buffer!");
		
		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_RenderPass;
		renderPassBeginInfo.framebuffer = m_Framebuffers[index];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 2> clearVals{};
		clearVals[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearVals[1].depthStencil = { 1.0f, 0 };

		renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearVals.size());
		renderPassBeginInfo.pClearValues = clearVals.data();

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		
		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
		
		VkBuffer vertexBuffers[] = { m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescSets[m_CurrentFrame], 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
		
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

		vkCmdEndRenderPass(commandBuffer);
		

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");
	}

	void App::DrawFrame() {
		vkWaitForFences(m_Device, 1, &m_IFFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imgInd;
		VkResult res = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imgInd);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (res != VK_SUCCESS)
			throw::std::runtime_error("Failed to acquire swap chain image");

		vkResetFences(m_Device, 1, &m_IFFences[m_CurrentFrame]);

		RecCommandBuffer(m_CommandBuffers[m_CurrentFrame], imgInd);

		UpdateUniformBuffer(m_CurrentFrame);

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

		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_IFFences[m_CurrentFrame]) != VK_SUCCESS)
			throw std::runtime_error("Failed to submit draw command buffer!");

		VkSwapchainKHR swapChains[] = { m_SwapChain };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imgInd;

		res = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || m_PWindow->framebufferResized) {
			m_PWindow->framebufferResized = false;
			RecreateSwapChain();
		}
		else if (res != VK_SUCCESS)
			throw std::runtime_error("Failed to present swap chain image!");

		m_CurrentFrame = (m_CurrentFrame + 1) % s_MaxFramesInFlight;
	}

	void App::UpdateUniformBuffer(uint32_t currentFrame) {
		static auto startTime	= std::chrono::high_resolution_clock::now();
		auto		currentTime	= std::chrono::high_resolution_clock::now();
		float		deltaTime	= std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
		UniformBufferObject uniBuff{};
		uniBuff.model = glm::rotate(
			  glm::mat4(1.0f)
			, deltaTime * glm::radians(10.0f)
			, glm::vec3(0.0f, 0.0f, 1.0f)
		);

		uniBuff.view = glm::lookAt(
			  glm::vec3(2.0f, 0.0f, 2.0f)
			, glm::vec3(0.0f, 0.0f, 0.0f)
			, glm::vec3(0.0f, 0.0f, 1.0f)
		);

		uniBuff.proj = glm::perspective(
			  glm::radians(60.0f)
			, (float) m_SwapChainExtent.width / (float) m_SwapChainExtent.height
			,  0.1f, 10.0f
		);

		uniBuff.proj[1][1] *= -1;

		void* data;
		vkMapMemory(m_Device, m_UniformBuffersMem[currentFrame], 0, sizeof(uniBuff), 0, &data);
		memcpy(data, &uniBuff, sizeof(uniBuff));
		vkUnmapMemory(m_Device, m_UniformBuffersMem[currentFrame]);
	}

	uint32_t App::GetMemType(uint32_t memTypeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProps;
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memProps);
	
		for (uint32_t i = 0; i < memProps.memoryTypeCount; i++)
			if ((memTypeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		throw std::runtime_error("Failed to get memory type");
	}

	[[nodiscard]] VkShaderModule App::CreateShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module!");

		return shaderModule;
	}

	VkSurfaceFormatKHR App::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& elem : availableFormats)
			if (elem.format == VK_FORMAT_R8G8B8A8_SRGB && elem.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return elem;

		return availableFormats[0];
	}

	VkPresentModeKHR App::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& elem : availablePresentModes)
			if (elem == VK_PRESENT_MODE_MAILBOX_KHR)
				return elem;

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D App::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
		
		int width, height;
		glfwGetFramebufferSize(m_PWindow->GetGLFWwindowPointer(), &width, &height);

		VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
		return actualExtent;
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