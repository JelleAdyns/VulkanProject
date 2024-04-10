#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
class GP2Buffer
{
public:

	GP2Buffer() = default;
	~GP2Buffer() = default;
	GP2Buffer(const GP2Buffer& other) = default;
	GP2Buffer(GP2Buffer&& other) noexcept = default;
	GP2Buffer& operator=(const GP2Buffer& other) = default;
	GP2Buffer& operator=(GP2Buffer&& other) noexcept = default;

	void CreateBuffer(const VkDevice& device, const VkPhysicalDevice& physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

	const VkBuffer& GetVkBuffer() const;
	const VkDeviceMemory& GetVkBufferMemory() const;

	
	void Upload(VkDeviceSize typeSize, void* ubo);
	void Map(const VkDevice& device);
	
	void DestroyBuffer(const VkDevice& device);
	void BindAsVertexBuffer(const VkCommandBuffer& commandBuffer) const;
	void BindAsIndexBuffer(const VkCommandBuffer& commandBuffer) const;
private:
	uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;
	VkDeviceSize m_DeviceSize;
	VkBuffer m_Buffer;
	VkDeviceMemory m_BufferMemory;
	void* m_pDataUniformBuffer;
};
