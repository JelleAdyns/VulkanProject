#pragma once

#include "GP2CommandBuffer.h"


struct QueueFamilyIndices;
class GP2CommandPool
{
public:
	GP2CommandPool() :
		m_CommandPool{ VK_NULL_HANDLE }
	{}

	~GP2CommandPool() = default;

	GP2CommandPool(const GP2CommandPool& other) = delete;
	GP2CommandPool(GP2CommandPool&& other) noexcept = delete;
	GP2CommandPool& operator=(const GP2CommandPool& other) = delete;
	GP2CommandPool& operator=(GP2CommandPool&& other) noexcept = delete;

	void Initialize(const VkDevice& device, const QueueFamilyIndices& familyIndices);

	const VkCommandPool& GetCommandPool() const{ return m_CommandPool; }
	void DestroyCommandPool(const VkDevice& device);
	GP2CommandBuffer createCommandBuffer(const VkDevice& device) const;
private:

	VkCommandPool m_CommandPool;
};
