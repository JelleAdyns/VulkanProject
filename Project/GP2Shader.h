#pragma once
#include "vulkanbase/VulkanUtil.h"
#include <array>
#include "Vertex.h"
#include "GP2Buffer.h"
#include "GP2DescriptorPool.h"
class GP2Shader final 
{
public:
	GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) :
		m_VertexShaderFile{ vertexShaderFile },
		m_FragmentShaderFile{ fragmentShaderFile },
		m_VecShadersStageInfos{  },
		m_UniformBuffer{},
		m_UBOSrc{},
		m_DescriptorSetLayout{},
		m_InputBinding{Vertex2D::GetBindingDescription()},
		m_AttributeDescriptions{ Vertex2D::GetAttributeDescriptions() }
	{}

	~GP2Shader() = default;

	GP2Shader(const GP2Shader& other) = delete;
	GP2Shader(GP2Shader&& other) noexcept = delete;
	GP2Shader& operator=(const GP2Shader& other) = delete;
	GP2Shader& operator=(GP2Shader&& other) noexcept = delete;
	
	void Init(const VkDevice& device, const VkPhysicalDevice& physicalDevice);
	void DestroyShaderModules(const VkDevice& device);
	void DestroyUniformObjects(const VkDevice& device);

	const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageInfos() { return m_VecShadersStageInfos; }
	
	void UpdateUniformBuffer(uint32_t currentImage, float aspectRatio, float fov);
	void BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index);

	void CreateDescriptorSets(const VkDevice& vkDevice);
	void CreateDescriptorSetLayout(const VkDevice& vkDevice);
	const VkDescriptorSetLayout& GetDescriptorSetLayout() { return m_DescriptorSetLayout; }

	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();
private:
	std::vector<VkPipelineShaderStageCreateInfo> m_VecShadersStageInfos;

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	GP2Buffer m_UniformBuffer;
	VertexUBO m_UBOSrc;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	std::unique_ptr<GP2DescriptorPool> m_DescriptorPool;

	std::array<VkVertexInputAttributeDescription, 2> m_AttributeDescriptions;
	VkVertexInputBindingDescription m_InputBinding;
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& device);
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& device);

	VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& code);
};
