#pragma once
#include "vulkanbase/VulkanUtil.h"

class GP2CommandPool;
class GP2CommandBuffer final
{
public:

	GP2CommandBuffer() = default;
	~GP2CommandBuffer() = default;
	GP2CommandBuffer(const GP2CommandBuffer& other) = default;
	GP2CommandBuffer(GP2CommandBuffer&& other) noexcept = default;
	GP2CommandBuffer& operator=(const GP2CommandBuffer& other) = default;
	GP2CommandBuffer& operator=(GP2CommandBuffer&& other) noexcept = default;

	void GP2CommandBuffer::submit(VkSubmitInfo& info)const;
	void BeginRecordBuffer();
	void EndRecordBuffer();
	void reset() const;

	void FreeBuffer(const VkDevice& device, const GP2CommandPool& commandPool);

	void SetVKCommandBuffer(const VkCommandBuffer& vkCommandBuffer) { m_CommandBuffer = vkCommandBuffer; }
	VkCommandBuffer GetVkCommandBuffer()const { return m_CommandBuffer;}
private:

	VkCommandBuffer m_CommandBuffer;
};
