#pragma once
#include "GP2Shader.h"
//class GP2Shader3D final : public GP2Shader
//{
//public:
//	GP2Shader3D(const std::string& vertexShaderFile, const std::string& fragmentShaderFile);
//	virtual ~GP2Shader3D() = default;
//
//	GP2Shader3D(const GP2Shader3D& other) = delete;
//	GP2Shader3D(GP2Shader3D&& other) noexcept = delete;
//	GP2Shader3D& operator=(const GP2Shader3D& other) = delete;
//	GP2Shader3D& operator=(GP2Shader3D&& other) noexcept = delete;
//
//	void UpdateUniformBuffer(uint32_t currentImage, glm::mat4 view, glm::mat4 projection);
//
//	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo();
//private:
//
//	std::array<VkVertexInputAttributeDescription, 3> m_AttributeDescriptions;
//	VkVertexInputBindingDescription m_InputBinding;
//};
