#pragma once

#include <vulkan/vulkan_core.h>

namespace Render {

	// Forward declare
	class GDevice;

	class GBuffer {
	public:
		GBuffer() = default;
		GBuffer(const GBuffer& rhs)
			: m_Buffer(rhs.m_Buffer)
			, m_BufferMem(rhs.m_BufferMem)
			, m_Size(rhs.m_Size)
			, m_UsageFlags(rhs.m_UsageFlags)
			, m_MemFlags(rhs.m_MemFlags) {};


		// Creates VkBuffer
		static void CreateBuffer(
			  GDevice* device
			, VkDeviceSize size
			, VkBufferUsageFlags usgFlags
			, VkMemoryPropertyFlags props
			, VkBuffer& buffer
			, VkDeviceMemory& bufferMem
		);

		// Copies VkBuffer
		static void CopyBuffer(
			  GDevice* device
			, VkBuffer srcBuffer
			, VkBuffer dstBuffer
			, VkDeviceSize size
		);


		inline VkBuffer&				GetVkBuffer() { return m_Buffer; }
		inline VkDeviceMemory&			GetVkDeviceMem() { return m_BufferMem; }
		inline VkDeviceSize&			GetVkDeviceSize() { return m_Size; }
		inline VkBufferUsageFlags		GetVkBufferUsageFlags() { return m_UsageFlags; }
		inline VkMemoryPropertyFlags	GetVkMemFlags() { return m_MemFlags; }

	private:

		//GDevice& m_Device;

		VkBuffer m_Buffer			= VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMem	= VK_NULL_HANDLE;

		VkDeviceSize m_Size;
		VkBufferUsageFlags m_UsageFlags;
		VkMemoryPropertyFlags m_MemFlags;


	};

}
