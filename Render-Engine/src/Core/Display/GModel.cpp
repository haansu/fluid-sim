#include "pch.h"
//#include "GModel.h"
//#include <memory>
//
//// Vulkan
//#include <vulkan/vulkan.h>
////
//
//// TOL
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tiny_obj_loader.h>
////
//
//namespace Render {
//	/// <summary>
//	/// Loads vertices from specified file
//	/// </summary
//	/*std::unique_ptr<GModel> GModel::LoadModel(const std::string& path) {
//		tinyobj::attrib_t att;
//		std::vector<tinyobj::shape_t> shapes;
//		std::vector<tinyobj::material_t> materials;
//		std::string warn;
//		std::string err;
//
//		if (!tinyobj::LoadObj(&att, &shapes, &materials, &warn, &err, path.c_str()))
//			throw std::runtime_error(warn + err);
//
//		for (const auto& elem : shapes) {
//			for (const auto& index : elem.mesh.indices) {
//				Vertex vert{};
//
//				vert.pos = {
//					  att.vertices[3 * index.vertex_index + 0]
//					, att.vertices[3 * index.vertex_index + 1]
//					, att.vertices[3 * index.vertex_index + 2]
//				};
//
//				vert.uv = {
//					  att.texcoords[2 * index.texcoord_index + 0]
//					, 1.0f - att.texcoords[2 * index.texcoord_index + 1]
//				};
//
//				vert.color = { 1.0f, 1.0f, 1.0f, 1.0f };
//
//				m_Vertices.push_back(vert);
//				m_Indices.push_back((uint32_t)m_Indices.size());
//			}
//		}
//	}*/
//
//	/// <summary>
//	/// Creates vertex buffers
//	/// </summary>
//	/*void GModel::CreateVertexBuffers(const std::vector<Vertex>& vertices) {
//		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMem;
//
//		CreateBuffer(
//			m_Device
//			, bufferSize
//			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
//			, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
//			, stagingBuffer
//			, stagingBufferMem
//		);
//
//		void* data;
//		vkMapMemory(m_Device, stagingBufferMem, 0, bufferSize, 0, &data);
//		memcpy(data, m_Vertices.data(), (size_t)bufferSize);
//		vkUnmapMemory(m_Device, stagingBufferMem);
//
//		CreateBuffer(
//			m_Device
//			, bufferSize
//			, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
//			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
//			, m_VertexBuffer
//			, m_VertexBufferMem
//		);
//
//		CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);
//
//		vkDestroyBuffer(m_Device, stagingBuffer, nullptr);
//		vkFreeMemory(m_Device, stagingBufferMem, nullptr);
//	}*/
//
//}