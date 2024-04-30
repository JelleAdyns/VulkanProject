#include "GP2Buffer.h"
#include "vulkanBase/VulkanBase.h"

void GP2Buffer::CreateBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanBase::FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);

	m_DeviceSize = size;
}

void GP2Buffer::Upload(VkDeviceSize typeSize, void* ubo)
{
	memcpy(m_pDataUniformBuffer, ubo, typeSize);
}

void GP2Buffer::Map(const VkDevice& device)
{
	vkMapMemory(device, m_BufferMemory, 0, m_DeviceSize, 0, &m_pDataUniformBuffer);
}
const VkBuffer& GP2Buffer::GetVkBuffer() const
{
	return m_Buffer;
}

const VkDeviceMemory& GP2Buffer::GetVkBufferMemory() const
{
	return m_BufferMemory;
}

void GP2Buffer::DestroyBuffer(const VkDevice& device)
{
	vkDestroyBuffer(device, m_Buffer, nullptr);
	vkFreeMemory(device, m_BufferMemory, nullptr);
}

void GP2Buffer::BindAsVertexBuffer(const VkCommandBuffer& commandBuffer) const
{
	VkBuffer vertexBuffers[] = { m_Buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
}

void GP2Buffer::BindAsIndexBuffer(const VkCommandBuffer& commandBuffer) const
{
	vkCmdBindIndexBuffer(commandBuffer, m_Buffer, 0, VK_INDEX_TYPE_UINT32);
}

uint32_t GP2Buffer::FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

