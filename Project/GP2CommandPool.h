#pragma once

#include "vulkanbase/VulkanUtil.h"

struct QueueFamilyIndices;
class GP2CommandBuffer;
class GP2CommandPool
{
public:
	GP2CommandPool() :
		m_CommandPool{ }
	{}

	~GP2CommandPool() = default;

	GP2CommandPool(const GP2CommandPool& other) = delete;
	GP2CommandPool(GP2CommandPool&& other) noexcept = delete;
	GP2CommandPool& operator=(const GP2CommandPool& other) = delete;
	GP2CommandPool& operator=(GP2CommandPool&& other) noexcept = delete;

	void Initialize(const VkDevice& device, const QueueFamilyIndices& familyIndices);

	const VkCommandPool& GetCommandPool() const{ return m_CommandPool; }
	GP2CommandBuffer CreateCommandBuffer(const VkDevice& device) const;
	void DestroyCommandPool(const VkDevice& device);
private:

	VkCommandPool m_CommandPool;
};
