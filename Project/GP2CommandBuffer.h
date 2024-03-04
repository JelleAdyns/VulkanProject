#pragma once
#include "vulkanbase/VulkanUtil.h"

class GP2CommandBuffer final
{
public:

	GP2CommandBuffer() = default;
	~GP2CommandBuffer() = default;


	void GP2CommandBuffer::submit(VkSubmitInfo& info)const;
	void BeginRecordBuffer();
	void EndRecordBuffer();
	void reset() const;

	void SetVKCommandBuffer(const VkCommandBuffer& vkCommandBuffer) { m_CommandBuffer = vkCommandBuffer; }
	VkCommandBuffer GetVkCommandBuffer()const { return m_CommandBuffer;}
private:

	VkCommandBuffer m_CommandBuffer;
};
