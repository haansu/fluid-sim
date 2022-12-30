#include "pch.h"
#include "GModel.h"

#include "GBuffer.h"
#include "GDevice.h"

// Vulkan
#include <vulkan/vulkan.h>
//

// TOL
#include <tiny_obj_loader.h>
//

namespace Render {

	GModel::GModel(GDevice& device, const std::string& modelPath)
		: m_VertexBuffer(new GBuffer()), m_IndexBuffer(new GBuffer()) {

		m_Device = &device;
		LoadModel(modelPath);
		CreateVertexBuffers(m_Vertices);
		CreateIndexBuffers(m_Indices);		
	}

	GModel::~GModel() {};

	void GModel::LoadModel(const std::string& path) {
		tinyobj::attrib_t att;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn;
		std::string err;

		if (!tinyobj::LoadObj(&att, &shapes, &materials, &warn, &err, path.c_str()))
			throw std::runtime_error(warn + err);

		for (const auto& elem : shapes) {
			for (const auto& index : elem.mesh.indices) {
				Vertex vert{};

				vert.pos = {
					  att.vertices[3 * index.vertex_index + 0]
					, att.vertices[3 * index.vertex_index + 1]
					, att.vertices[3 * index.vertex_index + 2]
				};

				vert.uv = {
					  att.texcoords[2 * index.texcoord_index + 0]
					, 1.0f - att.texcoords[2 * index.texcoord_index + 1]
				};

				vert.color = { 1.0f, 1.0f, 1.0f, 1.0f };

				m_Vertices.push_back(vert);
				m_Indices.push_back((uint32_t)m_Indices.size());
			}
		}
	}

	void GModel::CreateVertexBuffers(const std::vector<Vertex>& vertices) {
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;

		GBuffer::CreateBuffer(
			  m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, stagingBuffer
			, stagingBufferMem
		);

		void* data;
		vkMapMemory(m_Device->GetDevice(), stagingBufferMem, 0, bufferSize, 0, &data);
		memcpy(data, m_Vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device->GetDevice(), stagingBufferMem);

		GBuffer::CreateBuffer(
			m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_VertexBuffer->GetVkBuffer()
			, m_VertexBufferMem
		);

		GBuffer::CopyBuffer(m_Device, stagingBuffer, m_VertexBuffer->GetVkBuffer(), bufferSize);

		vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), stagingBufferMem, nullptr);
	}

	void GModel::CreateIndexBuffers(const std::vector<uint32_t>& indices) {
		if (!m_HasIndexBuffer)
			return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMem;

		GBuffer::CreateBuffer(
			  m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, stagingBuffer
			, stagingBufferMem
		);

		void* data;
		vkMapMemory(m_Device->GetDevice(), stagingBufferMem, 0, bufferSize, 0, &data);
		memcpy(data, m_Indices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device->GetDevice(), stagingBufferMem);

		GBuffer::CreateBuffer(
			m_Device
			, bufferSize
			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_IndexBuffer->GetVkBuffer()
			, m_IndexBufferMem
		);

		GBuffer::CopyBuffer(m_Device, stagingBuffer, m_IndexBuffer->GetVkBuffer(), bufferSize);

		vkDestroyBuffer(m_Device->GetDevice(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), stagingBufferMem, nullptr);
	}

	/// <summary>
	/// Binds model to the command buffer
	/// </summary>
	void GModel::Bind(VkCommandBuffer& commBuffer, VkPipelineLayout& pipelineLayout, VkDescriptorSet& descSet) {
		VkBuffer buffers[] = {m_VertexBuffer->GetVkBuffer()};
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commBuffer, 0, 1, buffers, offsets);

		if (m_HasIndexBuffer)
			vkCmdBindIndexBuffer(commBuffer, m_IndexBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descSet, 0, nullptr);
	}

	/// <summary>
	/// Draws model
	/// </summary>
	void GModel::Draw(VkCommandBuffer& commBuffer) {
		if (!m_HasIndexBuffer) {
			vkCmdDraw(commBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
			return;
		}

		vkCmdDrawIndexed(commBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
	}

}






