#include "pch.h"
#include "UI.h"

// Core
#include <Core/Display/GDevice.h>
//

// ImGUI
#include <imgui.h>
#include <imconfig.h>
#include <imgui_tables.cpp>
#include <imgui_draw.cpp>
#include <imgui_demo.cpp>
#include <imgui_widgets.cpp>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
//

// GLM

//

// STL
#include <string>
#include <sstream>
#include <iomanip>
//

namespace Render {

	//
	float UI::cameraPosition[3] = { 0.0f, 0.0f, 0.0f };
	float UI::cameraRotation[3] = { 0.0f, 0.0f, 0.0f };
	float UI::cameraVelocity[3] = { 0.0f, 0.0f, 0.0f };
	int UI::cursorDelta[2] = { 0, 0 };
	uint32_t UI::fps = 0;
	int UI::canvasWidth = 0;
	int UI::canvasHeight = 0;



	// Initialize ImGUI for Vulkan
	void UI::Begin(
		GDevice* device
		, VkInstance& instance
		, VkRenderPass& renderPass
		, VkDescriptorPool& descPool,
		const uint8_t& maxFramesInFlight) {

		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForVulkan(device->GetWindow()->GetGLFWwindowPointer(), true);

		ImGui_ImplVulkan_InitInfo imguiVkInfo{};
		imguiVkInfo.Instance = instance;
		imguiVkInfo.Queue = device->GetGraphicsQueue();
		imguiVkInfo.DescriptorPool = descPool;
		imguiVkInfo.Device = device->GetDevice();
		imguiVkInfo.PhysicalDevice = device->GetPhysicalDevice();
		imguiVkInfo.ImageCount = maxFramesInFlight;
		imguiVkInfo.MinImageCount = 2;
		ImGui_ImplVulkan_Init(&imguiVkInfo, renderPass);

		VkCommandBuffer commBuff = device->BeginSTCommands();

		ImGui_ImplVulkan_CreateFontsTexture(commBuff);

		device->EndSTCommands(commBuff);

		vkDeviceWaitIdle(device->GetDevice());
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	void UI::End() {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void UI::Main(VkCommandBuffer& commBuffer) {
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		Camera();
		FPS();

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commBuffer);
	}

	void UI::Camera() {
		ImGui::Begin("Camera");
		ImGui::Text("Position: X: %f Y: %f Z: %f", cameraPosition[0], cameraPosition[1], cameraPosition[2]);
		ImGui::Text("Rotation: X: %f Y: %f Z: %f", cameraRotation[0], cameraRotation[1], cameraRotation[2]);
		ImGui::Text("Velocity: X: %f Y: %f Z: %f", cameraVelocity[0], cameraVelocity[1], cameraVelocity[2]);
		ImGui::Text("CurDelta: X: %d Y: %d", cursorDelta[0], cursorDelta[1]);
		ImGui::End();
	}

	void UI::FPS() {
		ImGui::Begin("FPS");
		ImGui::Text("%d", fps);
		ImGui::End();
	}

}