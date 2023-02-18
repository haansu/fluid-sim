#include "pch.h"
#include "GBuffer.h"
#include "GDevice.h"

#include <Core.h>

namespace Render {

	void GBuffer::CreateBuffer(
		  GDevice* device
		, VkDeviceSize size
		, VkBufferUsageFlags usgFlags
		, VkMemoryPropertyFlags props
		, VkBuffer& buffer
		, VkDeviceMemory& bufferMem) {

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usgFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device->GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to create vertex buffer!");

		VkMemoryRequirements bufferMemReq{};
		vkGetBufferMemoryRequirements(device->GetDevice(), buffer, &bufferMemReq);

		VkMemoryAllocateInfo mallocInfo{};
		mallocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		mallocInfo.allocationSize = bufferMemReq.size;
		mallocInfo.memoryTypeIndex = device->GetMemType(bufferMemReq.memoryTypeBits, props);

		if (vkAllocateMemory(device->GetDevice(), &mallocInfo, nullptr, &bufferMem) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate memory for the vertex buffer!");

		vkBindBufferMemory(device->GetDevice(), buffer, bufferMem, 0);
	}

	void GBuffer::CopyBuffer(
		  GDevice* device
		, VkBuffer srcBuffer
		, VkBuffer dstBuffer
		, VkDeviceSize size) {

		VkCommandBuffer cmdBuffer = device->BeginSTCommands();

		VkBufferCopy bufferCopy{};
		bufferCopy.size = size;
		bufferCopy.srcOffset = 0;
		bufferCopy.dstOffset = 0;
		vkCmdCopyBuffer(cmdBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

		device->EndSTCommands(cmdBuffer);
	}

}
