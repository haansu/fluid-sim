#pragma once

#include "../../Defs.h"
#include "../Graphics/Vertex.h"
#include <vulkan/vulkan_core.h>

namespace Render {

	class GModel {
	public:
		NO_COPY(GModel);

		std::unique_ptr<GModel> LoadModel(const std::string& path);
	
		void Bind(VkCommandBuffer maybe);
		void Draw(VkCommandBuffer maybe);
	private:
		
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

		VkDevice m_Device;
		
		std::unique_ptr<VkBuffer> m_VertexBuffer;
		std::unique_ptr<VkBuffer> m_IndexBuffer;
		
		VkDeviceMemory m_VertexBufferMem;
		VkDeviceMemory m_IndexBufferMem;
	
		uint32_t m_IndexCount;

		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;

		bool m_HasIndexBuffer = false;

	};

}
