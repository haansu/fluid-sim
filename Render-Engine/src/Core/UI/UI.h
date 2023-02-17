#pragma once

// Vulkan
#include <vulkan/vulkan_core.h>
//

namespace Render {

	class GDevice;

	/// <summary>
	/// Static UI Class
	/// </summary>
	class UI
	{
	public:
		static void Begin(
			  GDevice* device
			, VkInstance& instance
			, VkRenderPass& renderPass
			, VkDescriptorPool& descPool,
			const uint8_t& framesInFlight
		);

		static void End();

		// This method is the collection of all UI
		static void Main(VkCommandBuffer& commBuffer);

		// Camera controls
		static void Camera();

		static float cameraPos[3];

	};

}
