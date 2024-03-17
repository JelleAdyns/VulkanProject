#include "GP2Mesh.h"
#include "GP2CommandPool.h"
#include "GP2CommandBuffer.h"

void GP2Mesh::Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
{
	VkDeviceSize size = sizeof(m_VecVertices[0]) * m_VecVertices.size();
	
	m_StagingBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	void* data;
	vkMapMemory(device, m_StagingBuffer.GetVkBufferMemory(), 0, size, 0, &data);
	memcpy(data, m_VecVertices.data(), (size_t)size);
	vkUnmapMemory(device, m_StagingBuffer.GetVkBufferMemory());
	
	m_VertexBuffer.CreateBuffer(device, physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(device, commandPool, size, graphicsQueue);
}

void GP2Mesh::DestroyMesh(const VkDevice& device)
{
	m_VertexBuffer.DestroyBuffer(device);
	m_StagingBuffer.DestroyBuffer(device);
}

void GP2Mesh::Draw(const VkCommandBuffer& cmdBuffer) const
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer.GetVkBuffer() };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdDraw(cmdBuffer, static_cast<uint32_t>(m_VecVertices.size()), 1, 0, 0);
}

void GP2Mesh::AddVertex(glm::vec2 pos, glm::vec3 color)
{
	m_VecVertices.push_back(Vertex{ pos, color });
}

void GP2Mesh::CopyBuffer(const VkDevice& device, const GP2CommandPool& commandPool, VkDeviceSize size, VkQueue graphicsQueue)
{
	GP2CommandBuffer cmdBuffer = commandPool.createCommandBuffer(device);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(cmdBuffer.GetVkCommandBuffer(), &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer.GetVkCommandBuffer(), m_StagingBuffer.GetVkBuffer(), m_VertexBuffer.GetVkBuffer(), 1, &copyRegion);

	vkEndCommandBuffer(cmdBuffer.GetVkCommandBuffer());

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	cmdBuffer.submit(submitInfo);

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	cmdBuffer.FreeBuffer(device, commandPool);

}

