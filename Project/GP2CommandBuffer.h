#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "ContextStructs.h"

class GP2CommandBuffer final
{
public:

	GP2CommandBuffer() = default;

	void Submit(VkSubmitInfo& info)const;
	void BeginRecordBuffer();
	void EndRecordBuffer();

	void BeginSingleTimeCommands();

	void EndSingleTimeCommands(const MeshContext& meshContext);
	void EndSingleTimeCommands(const VkQueue& graphicsQueue, const VkDevice& device, const GP2CommandPool& commandPool);

	void Reset() const;

	void FreeBuffer(const VkDevice& device, const GP2CommandPool& commandPool);

	void SetVKCommandBuffer(const VkCommandBuffer& vkCommandBuffer) { m_CommandBuffer = vkCommandBuffer; }
	VkCommandBuffer GetVkCommandBuffer()const { return m_CommandBuffer;}
private:

	VkCommandBuffer m_CommandBuffer;
};
