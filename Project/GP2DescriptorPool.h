#pragma once
#include <vector>
#include <array>
#include <memory>
#include "vulkanbase/VulkanUtil.h"
#include "GP2UniformBufferObject.h"
#include "GP2Texture.h"

template <typename UBO>
class GP2DescriptorPool
{
public:
	GP2DescriptorPool(const VkDevice& device, size_t count);
	~GP2DescriptorPool() = default;

	GP2DescriptorPool(const GP2DescriptorPool& other) = delete;
	GP2DescriptorPool(GP2DescriptorPool&& other) noexcept = default;
	GP2DescriptorPool& operator=(const GP2DescriptorPool& other) = delete;
	GP2DescriptorPool& operator=(GP2DescriptorPool&& other) noexcept = default;
	
	void Initialize(const VulkanContext& context, const GP2Texture& texture);
	
	void SetUBO(UBO data, size_t index);
	
	const VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
	
	void BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index);
	void DestroyPool(const VkDevice& device);

	void CreateDescriptorSets(const VkDevice& device, const GP2Texture& texture);
	
private:

	void CreateDescriptorSetLayout(const VkDevice& context);
	void CreateUBOs(const VulkanContext & context);
	
	size_t m_Count;
	VkDeviceSize m_Size;
	VkDescriptorPool m_pDescriptorPool;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::vector<VkDescriptorSet > m_DescriptorSets;
	std::vector< std::unique_ptr< GP2UniformBufferObject< UBO > > > m_UBOs;

	
};

template <typename UBO>
GP2DescriptorPool<UBO>::GP2DescriptorPool(const VkDevice& device, size_t count) :
	m_Count{ count },
	m_Size{ sizeof(UBO) },
	m_pDescriptorPool{},
	m_DescriptorSetLayout{},
	m_DescriptorSets{},
	m_UBOs{}
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(count);
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(count);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(count);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

template <typename UBO>
void GP2DescriptorPool<UBO>::Initialize(const VulkanContext& context, const GP2Texture& texture)
{
	CreateDescriptorSetLayout(context.device);
	CreateUBOs(context);
	CreateDescriptorSets(context.device, texture);
}
template <typename UBO>
void GP2DescriptorPool<UBO>::DestroyPool(const VkDevice& device)
{

	for (std::unique_ptr<GP2UniformBufferObject<UBO>>& buffer : m_UBOs)
	{
		buffer->DestroyBuffer(device);
	}
	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, m_pDescriptorPool, nullptr);

}
template <typename UBO>
void GP2DescriptorPool<UBO>::CreateDescriptorSets(const VkDevice& device, const GP2Texture& texture)
{
	std::vector<VkDescriptorSetLayout> layouts(m_Count, m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Count);
	allocInfo.pSetLayouts = layouts.data();

	m_DescriptorSets.resize(m_Count);
	if (vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	size_t i = 0;
	for (std::unique_ptr<GP2UniformBufferObject<UBO>>& buffer : m_UBOs)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = buffer->GetVkBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = m_Size;

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.GetImageView();
		imageInfo.sampler = texture.GetSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_DescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_DescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		++i;
	}

}

template <typename UBO>
void GP2DescriptorPool<UBO>::BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index)
{
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_DescriptorSets[index], 0, nullptr);
}

template <typename UBO>
void GP2DescriptorPool<UBO>::CreateDescriptorSetLayout(const VkDevice& vkDevice)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();


	if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

template<class UBO>
void GP2DescriptorPool<UBO>::CreateUBOs(const VulkanContext& context)
{
	for (int uboIndex = 0; uboIndex < m_Count; ++uboIndex)
	{
		auto buffer = std::make_unique<GP2UniformBufferObject<UBO>>();
		buffer->Initialize(context);
		m_UBOs.emplace_back(std::move(buffer));
	}
}

template<class UBO>
void GP2DescriptorPool<UBO>::SetUBO(UBO src, size_t index)
{
	if (index < m_UBOs.size())
	{
		m_UBOs[index]->SetData(src);
		m_UBOs[index]->Upload();
	}
}
