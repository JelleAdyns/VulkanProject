#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "GP2Buffer.h"
#include "GP2CommandPool.h"
#include "GP2CommandBuffer.h"
#include <vector>
#include "Vertex.h"

class GP2CommandPool;

template <typename VertexType>
class GP2Mesh final
{
public:
	
	GP2Mesh() = default;
	~GP2Mesh() = default;

	GP2Mesh(const GP2Mesh& other) = delete;
	GP2Mesh(GP2Mesh&& other) noexcept = default;
	GP2Mesh& operator=(const GP2Mesh& other) = delete;
	GP2Mesh& operator=(GP2Mesh&& other) noexcept = default;

	void DestroyMesh(const VkDevice& device);
	void Draw(const VkCommandBuffer& cmdBuffer) const;
	void UploadBuffers(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool,VkQueue graphicsQueue);

	void AddVertex(const VertexType& vertex);
	void AddIndex(uint32_t index);


private:

	void CopyBuffer(const VkDevice& device, const GP2CommandPool& commandPool, VkDeviceSize size, VkQueue graphicsQueue, VkBuffer src, VkBuffer dst);

	GP2Buffer m_VertexBuffer;
	GP2Buffer m_IndexBuffer;
	std::vector<VertexType> m_VecVertices;
	std::vector<uint32_t> m_VecIndices;
	//VertexConstant m_VertexConstant;
};

template <typename VertexType>
void GP2Mesh<VertexType>::UploadBuffers(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize size = sizeof(m_VecVertices[0]) * m_VecVertices.size();
	GP2Buffer stagingBuffer{};
	void* data;

	//VertexBuffer creation
	stagingBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, size, 0, &data);
	memcpy(data, m_VecVertices.data(), (size_t)size);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_VertexBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, commandPool, size, graphicsQueue, stagingBuffer.GetVkBuffer(), m_VertexBuffer.GetVkBuffer());

	stagingBuffer.DestroyBuffer(device);



	//IndexBuffer creation
	size = sizeof(m_VecIndices[0]) * m_VecIndices.size();

	stagingBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkMapMemory(device, stagingBuffer.GetVkBufferMemory(), 0, size, 0, &data);
	memcpy(data, m_VecIndices.data(), (size_t)size);
	vkUnmapMemory(device, stagingBuffer.GetVkBufferMemory());

	m_IndexBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, commandPool, size, graphicsQueue, stagingBuffer.GetVkBuffer(), m_IndexBuffer.GetVkBuffer());

	stagingBuffer.DestroyBuffer(device);
}

template <typename VertexType>
void GP2Mesh<VertexType>::DestroyMesh(const VkDevice& device)
{
	m_VertexBuffer.DestroyBuffer(device);
	m_IndexBuffer.DestroyBuffer(device);
}

template <typename VertexType>
void GP2Mesh<VertexType>::Draw(const VkCommandBuffer& cmdBuffer) const
{

	m_VertexBuffer.BindAsVertexBuffer(cmdBuffer);
	m_IndexBuffer.BindAsIndexBuffer(cmdBuffer);
	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_VecIndices.size()), 1, 0, 0, 0);
}

template <typename VertexType>
void GP2Mesh<VertexType>::CopyBuffer(const VkDevice& device, const GP2CommandPool& commandPool, VkDeviceSize size, VkQueue graphicsQueue, VkBuffer src, VkBuffer dst)
{
	GP2CommandBuffer cmdBuffer = commandPool.CreateCommandBuffer(device);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer.GetVkCommandBuffer(), &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer.GetVkCommandBuffer(), src, dst, 1, &copyRegion);

	vkEndCommandBuffer(cmdBuffer.GetVkCommandBuffer());

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	cmdBuffer.submit(submitInfo);

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	cmdBuffer.FreeBuffer(device, commandPool);

}

template <typename VertexType>
void GP2Mesh<VertexType>::AddVertex(const VertexType& vertex)
{
	m_VecVertices.push_back(vertex);
}

template <typename VertexType>
void GP2Mesh<VertexType>::AddIndex(uint32_t index)
{
	m_VecIndices.push_back(index);
}