#pragma once
#include <vector>
#include <array>
#include <memory>
#include "vulkanbase/VulkanUtil.h"
#include "GP2UniformBufferObject.h"
#include "GP2Texture.h"
#include "GP2Mesh.h"

template <typename UBO, typename VertexType>
class GP2DescriptorPool
{
public:
	GP2DescriptorPool(const VulkanContext& context, const std::vector<GP2Mesh<VertexType>>& vecMeshes);
	~GP2DescriptorPool() = default;

	GP2DescriptorPool(const GP2DescriptorPool& other) = delete;
	GP2DescriptorPool(GP2DescriptorPool&& other) noexcept = default;
	GP2DescriptorPool& operator=(const GP2DescriptorPool& other) = delete;
	GP2DescriptorPool& operator=(GP2DescriptorPool&& other) noexcept = default;
	
	void SetUBO(UBO data, size_t index);
	
	const VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
	
	void BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index);
	void DestroyPool(const VkDevice& device);

	
private:

	void CreateDescriptorSets(const VkDevice& device, const std::vector<GP2Mesh<VertexType>>& vecMeshes);
	void CreateDescriptorSetLayout(const VkDevice& context);
	void CreateUBOs(const VulkanContext & context);
	
	size_t m_Count;
	VkDeviceSize m_Size;
	VkDescriptorPool m_pDescriptorPool;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::vector<VkDescriptorSet > m_DescriptorSets;
	std::unique_ptr< GP2UniformBufferObject< UBO > > m_UBO;
	
};

template <typename UBO, typename VertexType>
GP2DescriptorPool<UBO, VertexType>::GP2DescriptorPool(const VulkanContext& context, const std::vector<GP2Mesh<VertexType>>& vecMeshes) :
	m_Count{ vecMeshes.size()},
	m_Size{ sizeof(UBO) },
	m_pDescriptorPool{},
	m_DescriptorSetLayout{},
	m_DescriptorSets{},
	m_UBO{}
{
	std::vector<VkDescriptorPoolSize> poolSizes{};

	VkDescriptorPoolSize ubo{};
	ubo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ubo.descriptorCount = 1;
	poolSizes.push_back(ubo);

	VkDescriptorPoolSize diffuseTexture{};
	diffuseTexture.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	diffuseTexture.descriptorCount = 1;
	poolSizes.push_back(diffuseTexture);

	VkDescriptorPoolSize normalTexture{};
	normalTexture.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalTexture.descriptorCount = 1;
	poolSizes.push_back(normalTexture);

	VkDescriptorPoolSize roughnessTexture{};
	roughnessTexture.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	roughnessTexture.descriptorCount = 1;
	poolSizes.push_back(roughnessTexture);
			  
	VkDescriptorPoolSize specularTexture{};
	specularTexture.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	specularTexture.descriptorCount = 1;
	poolSizes.push_back(specularTexture);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(m_Count);

	if (vkCreateDescriptorPool(context.device, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}

	m_DescriptorSets.resize(m_Count);
	
	CreateDescriptorSetLayout(context.device);
	CreateUBOs(context);
	CreateDescriptorSets(context.device, vecMeshes);
}

template <typename UBO, typename VertexType>
void GP2DescriptorPool<UBO, VertexType>::DestroyPool(const VkDevice& device)
{

	m_UBO->DestroyBuffer(device);
	
	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	vkDestroyDescriptorPool(device, m_pDescriptorPool, nullptr);

}
template <typename UBO, typename VertexType>
void GP2DescriptorPool<UBO, VertexType>::CreateDescriptorSets(const VkDevice& device, const std::vector<GP2Mesh<VertexType>>& vecMeshes)
{
	std::vector<VkDescriptorSetLayout> layouts(m_Count, m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_pDescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Count);
	allocInfo.pSetLayouts = layouts.data();


	if (vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = m_UBO->GetVkBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = m_Size;

	VkWriteDescriptorSet bufferWrite{};

	bufferWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	bufferWrite.dstSet = m_DescriptorSets[0];
	bufferWrite.dstBinding = 0;
	bufferWrite.dstArrayElement = 0;
	bufferWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bufferWrite.descriptorCount = 1;
	bufferWrite.pBufferInfo = &bufferInfo;

	size_t i = 0;
	//for (std::unique_ptr<GP2UniformBufferObject<UBO>>& buffer : m_UBOs)
	for (const auto& mesh : vecMeshes)
	{
		bufferWrite.dstSet = m_DescriptorSets[i];

		const auto& pMaterial = mesh.GetMaterial();

		VkDescriptorImageInfo diffuseTexInfo{};
		diffuseTexInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		diffuseTexInfo.imageView = pMaterial->m_Diffuse->GetImageView();
		diffuseTexInfo.sampler = pMaterial->m_Diffuse->GetSampler();


		VkWriteDescriptorSet diffuseTexWrite{};
		diffuseTexWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		diffuseTexWrite.dstSet = m_DescriptorSets[i];
		diffuseTexWrite.dstBinding = 1;
		diffuseTexWrite.dstArrayElement = 0;
		diffuseTexWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		diffuseTexWrite.descriptorCount = 1;
		diffuseTexWrite.pImageInfo = &diffuseTexInfo;

		VkDescriptorImageInfo normalTexInfo{};
		normalTexInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		normalTexInfo.imageView = pMaterial->m_Normal->GetImageView();
		normalTexInfo.sampler = pMaterial->m_Normal->GetSampler();

		VkWriteDescriptorSet normalTexWrite{};
		normalTexWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		normalTexWrite.dstSet = m_DescriptorSets[i];
		normalTexWrite.dstBinding = 2;
		normalTexWrite.dstArrayElement = 0;
		normalTexWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalTexWrite.descriptorCount = 1;
		normalTexWrite.pImageInfo = &normalTexInfo;

		VkDescriptorImageInfo roughnessTexInfo{};
		roughnessTexInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		roughnessTexInfo.imageView = pMaterial->m_Roughness->GetImageView();
		roughnessTexInfo.sampler = pMaterial->m_Roughness->GetSampler();

		VkWriteDescriptorSet roughnessTexWrite{};
		roughnessTexWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		roughnessTexWrite.dstSet = m_DescriptorSets[i];
		roughnessTexWrite.dstBinding = 3;
		roughnessTexWrite.dstArrayElement = 0;
		roughnessTexWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		roughnessTexWrite.descriptorCount = 1;
		roughnessTexWrite.pImageInfo = &roughnessTexInfo;

		VkDescriptorImageInfo specularTexInfo{};
		specularTexInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		specularTexInfo.imageView = pMaterial->m_Specular->GetImageView();
		specularTexInfo.sampler = pMaterial->m_Specular->GetSampler();

		VkWriteDescriptorSet specularTexWrite{};
		specularTexWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		specularTexWrite.dstSet = m_DescriptorSets[i];
		specularTexWrite.dstBinding = 4;
		specularTexWrite.dstArrayElement = 0;
		specularTexWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		specularTexWrite.descriptorCount = 1;
		specularTexWrite.pImageInfo = &specularTexInfo;

		std::vector<VkWriteDescriptorSet> descriptorWrites{ bufferWrite, diffuseTexWrite, normalTexWrite, roughnessTexWrite, specularTexWrite };

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		++i;
	}

}

template <typename UBO, typename VertexType>
void GP2DescriptorPool<UBO, VertexType>::BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index)
{
	vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &m_DescriptorSets[index], 0, nullptr);
}

template <typename UBO, typename VertexType>
void GP2DescriptorPool<UBO, VertexType>::CreateDescriptorSetLayout(const VkDevice& vkDevice)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

	VkDescriptorSetLayoutBinding diffuseLayoutBinding{};
	diffuseLayoutBinding.binding = 1;
	diffuseLayoutBinding.descriptorCount = 1;
	diffuseLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	diffuseLayoutBinding.pImmutableSamplers = nullptr;
	diffuseLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding normalLayoutBinding{};
	normalLayoutBinding.binding = 2;
	normalLayoutBinding.descriptorCount = 1;
	normalLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	normalLayoutBinding.pImmutableSamplers = nullptr;
	normalLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding roughnessLayoutBinding{};
	roughnessLayoutBinding.binding = 3;
	roughnessLayoutBinding.descriptorCount = 1;
	roughnessLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	roughnessLayoutBinding.pImmutableSamplers = nullptr;
	roughnessLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding specularLayoutBinding{};
	specularLayoutBinding.binding = 4;
	specularLayoutBinding.descriptorCount = 1;
	specularLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	specularLayoutBinding.pImmutableSamplers = nullptr;
	specularLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, diffuseLayoutBinding, normalLayoutBinding, roughnessLayoutBinding, specularLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();


	if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

template <typename UBO, typename VertexType>
void GP2DescriptorPool<UBO, VertexType>::CreateUBOs(const VulkanContext& context)
{
	m_UBO = std::make_unique<GP2UniformBufferObject<UBO>>();
	m_UBO->Initialize(context);
}

template <typename UBO, typename VertexType>
void GP2DescriptorPool<UBO, VertexType>::SetUBO(UBO src, size_t index)
{

	m_UBO->SetData(src);
	m_UBO->Upload();
}
