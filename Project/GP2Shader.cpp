#include "GP2Shader.h"



void GP2Shader::Init(const VkDevice& device, const VkPhysicalDevice& physicalDevice)
{
	m_VecShadersStageInfos.push_back(CreateFragmentShaderInfo(device));
	m_VecShadersStageInfos.push_back(CreateVertexShaderInfo(device));
	m_UniformBuffer.CreateBuffer(device, physicalDevice, sizeof(VertexUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_UniformBuffer.Map(device);

	m_DescriptorPool = std::make_unique<GP2DescriptorPool>( device, 0, 1 );
}
void GP2Shader::DestroyShaderModules(const VkDevice& device)
{
	for (VkPipelineShaderStageCreateInfo& stageInfo : m_VecShadersStageInfos)
	{
		vkDestroyShaderModule(device, stageInfo.module, nullptr);
	}
	m_VecShadersStageInfos.clear();
}
void GP2Shader::DestroyUniformObjects(const VkDevice& device)
{
	vkDestroyBuffer(device, m_UniformBuffer.GetVkBuffer(), nullptr);
	vkFreeMemory(device, m_UniformBuffer.GetVkBufferMemory(), nullptr);
	
	vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
	m_DescriptorPool->DestroyPool(device);
}
void GP2Shader::UploadUBOMatrix(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection)
{
	m_UBOSrc.model = model;
	m_UBOSrc.view = view;
	m_UBOSrc.proj = projection;

	m_UniformBuffer.Upload(m_UBOSrc);
}
void GP2Shader::BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index)
{
	m_DescriptorPool->BindDescriptorSet(buffer, layout, index);
}
void GP2Shader::CreateDescriptorSets(const VkDevice& vkDevice)
{
	m_DescriptorPool->CreateDescriptorSets(vkDevice, m_DescriptorSetLayout, { m_UniformBuffer.GetVkBuffer() });
}
void GP2Shader::CreateDescriptorSetLayout(const VkDevice& vkDevice)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
	
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	
	if (vkCreateDescriptorSetLayout(vkDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}
VkPipelineInputAssemblyStateCreateInfo GP2Shader::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}


VkPipelineShaderStageCreateInfo GP2Shader::CreateFragmentShaderInfo(const VkDevice& device)
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

VkPipelineShaderStageCreateInfo GP2Shader::CreateVertexShaderInfo(const VkDevice& device)
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

VkShaderModule GP2Shader::CreateShaderModule(const VkDevice& device, const std::vector<char>& code)
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