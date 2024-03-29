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

		// FPS
		static void FPS();

		// Simulation
		static void Simulation();

		static uint32_t fps;
		static float cameraPosition[3];
		static float cameraRotation[3];
		static float cameraVelocity[3];
		static int cursorDelta[2];

		static int canvasWidth;
		static int canvasHeight;

		static bool bReset;
		static bool bGravity;
		static bool bCollisions;
		static float viscosity;
		static float restDesnity;
		static float damping;
		static float stiffness;

		static int particleCount;
		static int xSpeed;
		static int ySpeed;
		static int zSpeed;

	};

}
