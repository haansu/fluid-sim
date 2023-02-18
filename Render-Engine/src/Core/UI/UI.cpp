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

namespace Render {

	//
	float UI::cameraPos[3] = { 0.0f, 0.0f, 0.0f };


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

		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commBuffer);
	}

	void UI::Camera() {
		ImGui::Begin("Camera");

		//static float cameraPos[3];
		ImGui::SliderFloat3("Camera XYZ", cameraPos, -100.0f, 100.0f);

		ImGui::End();
	}

}