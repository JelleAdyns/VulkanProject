#include "GP2CommandBuffer.h"

void GP2CommandBuffer::Reset()const 
{
	 vkResetCommandBuffer(m_CommandBuffer, /*VkCommandBufferResetFlagBits*/ 0);	
}

void GP2CommandBuffer::BeginSingleTimeCommands()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
}
void GP2CommandBuffer::EndSingleTimeCommands(const MeshContext& meshContext)
{
	EndSingleTimeCommands(meshContext.graphicsQueue, meshContext.device, meshContext.commandPool);
}
void GP2CommandBuffer::EndSingleTimeCommands(const VkQueue& graphicsQueue, const VkDevice& device, const GP2CommandPool& commandPool)
{
	EndRecordBuffer();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	Submit(submitInfo);
	
	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	FreeBuffer(device, commandPool);
}

void GP2CommandBuffer::FreeBuffer(const VkDevice& device, const GP2CommandPool& commandPool)
{
	vkFreeCommandBuffers(device, commandPool.GetCommandPool(), 1, &m_CommandBuffer);
}


void GP2CommandBuffer::BeginRecordBuffer()
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}
}
void GP2CommandBuffer::EndRecordBuffer()
{
	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void GP2CommandBuffer::Submit(VkSubmitInfo& info)const
{
	info.commandBufferCount = 1;
	info.pCommandBuffers = &m_CommandBuffer;
}

