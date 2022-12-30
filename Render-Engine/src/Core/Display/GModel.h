#pragma once

#include "../../Defs.h"
#include "../Graphics/Vertex.h"
#include <vulkan/vulkan_core.h>

namespace Render {

	// Forward declare
	class GBuffer;
	class GDevice;

	class GModel {
	public:
		NO_COPY(GModel);

		GModel(GDevice& device, const std::string& modelPath);
		~GModel();

		void LoadModel(const std::string& path);
	
		void Bind(VkCommandBuffer& commBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descSet);
		void Draw(VkCommandBuffer& commBuffer);
	private:
		
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		GDevice* m_Device;
		
		std::unique_ptr<GBuffer> m_VertexBuffer;
		std::unique_ptr<GBuffer> m_IndexBuffer;
		
		VkDeviceMemory m_VertexBufferMem;
		VkDeviceMemory m_IndexBufferMem;
	
		uint32_t m_IndexCount;
		uint32_t m_VertexCount;

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		bool m_HasIndexBuffer = false;
	};

}
