#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "GP2CommandPool.h"

struct MeshContext
{
	const VkDevice& device;
	const VkPhysicalDevice& physicalDevice;
	const GP2CommandPool& commandPool;
	const VkQueue& graphicsQueue;
};

struct VulkanContext
{
	const VkDevice& device;
	const VkPhysicalDevice& physicalDevice;
	const VkRenderPass& renderPass;
	const VkExtent2D& swapChainExtent;
};
