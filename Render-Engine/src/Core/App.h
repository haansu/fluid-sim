#pragma once

#include <vector>

#define RENDER_API		__declspec(dllexport)
#define public_var		public
#define private_var		private

#define VKAPI_ATTR
#define VKAPI_CALL		__stdcall
#define VK_NULL_HANDLE	nullptr

// Forward declared Vulkan

struct VkInstance_T;
struct VkSurfaceKHR_T;
struct VkDevice_T;
struct VkPhysicalDevice_T;
struct VkQueue_T;
struct VkImage_T;
struct VkImageView_T;
struct VkShaderModule_T;
struct VkSwapchainKHR_T;
struct VkDebugUtilsMessengerEXT_T;

typedef VkInstance_T*				VkInstance;
typedef VkSurfaceKHR_T*				VkSurfaceKHR;
typedef VkDevice_T*					VkDevice;
typedef VkPhysicalDevice_T*			VkPhysicalDevice;
typedef VkQueue_T*					VkQueue;
typedef VkImage_T*					VkImage;
typedef VkImageView_T*				VkImageView;
typedef VkShaderModule_T*			VkShaderModule;
typedef VkSwapchainKHR_T*			VkSwapchainKHR;
typedef VkDebugUtilsMessengerEXT_T* VkDebugUtilsMessengerEXT;

typedef uint32_t VkFlags;
typedef uint32_t VkBool32;
typedef VkFlags VkDebugUtilsMessageTypeFlagsEXT;

struct VkSurfaceFormatKHR;
struct VkExtent2D;
struct VkDebugUtilsMessengerCreateInfoEXT;
struct VkDebugUtilsMessengerCallbackDataEXT;
struct VkSurfaceCapabilitiesKHR;

enum VkFormat;
enum VkDebugUtilsMessageSeverityFlagBitsEXT;
enum VkPresentModeKHR;
enum VkPresentModeKHR;

namespace Render {

	// Forward declared Local

	class Window;
	struct QFamilyInd;
	struct SwapChainSupportDetails;

	class App final {
	public:
		RENDER_API void Run();

	private:
		void Init();
		void MainLoop();
		void CleanUp();
		void CreateInstance();

		void InitObjects();

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
		void CreateImageViews();
		void CreateGraphicsPipeline();

		VkShaderModule CreateShaderModule(const std::vector<char>& code);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		bool CheckDeviceExtSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

	private_var:
		Window* m_PWindow;

		VkInstance m_VkInstance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device;

		VkSurfaceKHR m_Surface;

		VkSwapchainKHR m_SwapChain;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D* m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VkQueue m_GraphicsQueue;
		VkQueue	m_PresentQueue;
	};

}