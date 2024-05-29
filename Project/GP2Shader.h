#pragma once
#include "vulkanbase/VulkanUtil.h"
#include <array>
#include "Vertex.h"
#include "GP2Buffer.h"
#include "GP2DescriptorPool.h"
#include "ContextStructs.h"

template <typename VertexType>
class GP2Shader final
{
public:
	
	GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) :
		m_VertexShaderFile{ vertexShaderFile },
		m_FragmentShaderFile{ fragmentShaderFile },
		m_VecShadersStageInfos{  },
		m_InputBinding{ VertexType::GetBindingDescription() }
	{
		auto inputAttributes = VertexType::GetAttributeDescriptions();
		for (int i = 0; i < inputAttributes.size(); i++) m_AttributeDescriptions.push_back(inputAttributes[i]);
	}

	virtual ~GP2Shader() = default;

	GP2Shader(const GP2Shader& other) = delete;
	GP2Shader(GP2Shader&& other) noexcept = delete;
	GP2Shader& operator=(const GP2Shader& other) = delete;
	GP2Shader& operator=(GP2Shader&& other) noexcept = delete;
	
	void Init(const VulkanContext& vulkanContext);
	void DestroyShaderModules(const VkDevice& device);

	const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageInfos() const { return m_VecShadersStageInfos; }


	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();
	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();


private:
	std::vector<VkPipelineShaderStageCreateInfo> m_VecShadersStageInfos;

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;



	std::vector<VkVertexInputAttributeDescription> m_AttributeDescriptions;
	VkVertexInputBindingDescription m_InputBinding;

	VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo(const VkDevice& device);
	VkPipelineShaderStageCreateInfo CreateVertexShaderInfo(const VkDevice& device);

	VkShaderModule CreateShaderModule(const VkDevice& device, const std::vector<char>& code);
};

template <typename VertexType>
void GP2Shader<VertexType>::Init(const VulkanContext& vulkanContext)
{
	m_VecShadersStageInfos.push_back(CreateVertexShaderInfo(vulkanContext.device));
	m_VecShadersStageInfos.push_back(CreateFragmentShaderInfo(vulkanContext.device));
}
template <typename VertexType>
void GP2Shader<VertexType>::DestroyShaderModules(const VkDevice& device)
{
	for (VkPipelineShaderStageCreateInfo& stageInfo : m_VecShadersStageInfos)
	{
		vkDestroyShaderModule(device, stageInfo.module, nullptr);
	}
	m_VecShadersStageInfos.clear();
}

template <typename VertexType>
VkPipelineInputAssemblyStateCreateInfo GP2Shader<VertexType>::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

template <typename VertexType>
VkPipelineVertexInputStateCreateInfo GP2Shader<VertexType>::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ };

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &m_InputBinding;
	vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
	return vertexInputInfo;
}

template <typename VertexType>
VkPipelineShaderStageCreateInfo GP2Shader<VertexType>::CreateFragmentShaderInfo(const VkDevice& device)
{
	std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
	VkShaderModule fragShaderModule = CreateShaderModule(device, fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

template <typename VertexType>
VkPipelineShaderStageCreateInfo GP2Shader<VertexType>::CreateVertexShaderInfo(const VkDevice& device)
{
	std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
	VkShaderModule vertShaderModule = CreateShaderModule(device, vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	return vertShaderStageInfo;
}

template <typename VertexType>
VkShaderModule GP2Shader<VertexType>::CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}