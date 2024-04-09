#include "GP2Shader3D.h"
//#define GLM_FORCE_LEFT_HANDED
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

GP2Shader3D::GP2Shader3D(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) :
	GP2Shader{ vertexShaderFile, fragmentShaderFile },
	m_InputBinding{ Vertex3D::GetBindingDescription() },
	m_AttributeDescriptions{ Vertex3D::GetAttributeDescriptions() }
{

}

//void GP2Shader3D::UpdateUniformBuffer(uint32_t currentImage, float aspectRatio, float fov)
void GP2Shader3D::UpdateUniformBuffer(uint32_t currentImage, glm::mat4 view, glm::mat4 projection)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	auto model = glm::rotate(glm::mat4(1.0f), time * glm::radians(00.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//auto view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//auto proj = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 10.0f);
	//proj[1].y *= -1;

	UploadUBOMatrix(model, view, projection);
}

VkPipelineVertexInputStateCreateInfo GP2Shader3D::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ };

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &m_InputBinding;
	vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
	return vertexInputInfo;
}