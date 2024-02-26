#pragma once
#include "vulkanbase/VulkanUtil.h"
class GP2Shader final 
{
public:
	GP2Shader(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) : 
		m_VertexShaderFile{ vertexShaderFile },
		m_FragmentShaderFile{ fragmentShaderFile },
		m_VecShadersStageInfos{}
	{}

	~GP2Shader() = default;

	GP2Shader(const GP2Shader& other) = delete;
	GP2Shader(GP2Shader&& other) noexcept = delete;
	GP2Shader& operator=(const GP2Shader& other) = delete;
	GP2Shader& operator=(GP2Shader&& other) noexcept = delete;
	
	void Init(const VkDevice& device);

	const std::vector<VkPipelineShaderStageCreateInfo>& GetShaderStageInfos() { return m_VecShadersStageInfos; }
	void DestroyShaderModules(const VkDevice& device);

	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();
private:
	std::vector<VkPipelineShaderStageCreateInfo> m_VecShadersStageInfos;

	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& device);
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& device);

	VkShaderModule createShaderModule(const VkDevice& device, const std::vector<char>& code);
};
