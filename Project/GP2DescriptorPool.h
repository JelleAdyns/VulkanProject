#pragma once
#include <vector>
#include "vulkanbase/VulkanUtil.h"

class GP2DescriptorPool
{
public:
	GP2DescriptorPool(const VkDevice& device, VkDeviceSize size, size_t count);
	~GP2DescriptorPool() = default;

	GP2DescriptorPool(const GP2DescriptorPool& other) = delete;
	GP2DescriptorPool(GP2DescriptorPool&& other) noexcept = default;
	GP2DescriptorPool& operator=(const GP2DescriptorPool& other) = delete;
	GP2DescriptorPool& operator=(GP2DescriptorPool&& other) noexcept = default;


	void DestroyPool(const VkDevice& device);

	void CreateDescriptorSets(const VkDevice& device, VkDescriptorSetLayout descriptorSetLayout, std::initializer_list<VkBuffer > buffers);
	
	void BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index);
	
 private:

	size_t m_Count;
	VkDeviceSize m_Size;
	VkDescriptorPool m_DescriptorPool;
	std::vector<VkDescriptorSet > m_DescriptorSets;
	
};
