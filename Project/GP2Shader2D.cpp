#include "GP2Shader2D.h"
//#define GLM_FORCE_LEFT_HANDED
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

GP2Shader2D::GP2Shader2D(const std::string& vertexShaderFile, const std::string& fragmentShaderFile):
	GP2Shader{vertexShaderFile, fragmentShaderFile},
	m_InputBinding{ Vertex2D::GetBindingDescription() },
	m_AttributeDescriptions{ Vertex2D::GetAttributeDescriptions() }
{

}

void GP2Shader2D::UpdateUniformBuffer(uint32_t currentImage, float width, float height, float fov)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	glm::vec3 scaleFactors(1 / width, 1 / height, 1.0f);
	auto view = glm::scale(glm::mat4(1.0f), scaleFactors);
	view = glm::translate(view, glm::vec3(-1, -1, 0));

	auto model =  glm::rotate(glm::mat4(1.0f), time * glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//auto view = glm::mat4(1.0f);// glm::lookAt(glm::vec3(0, 0, 6), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	auto proj = glm::mat4(1.0f);//glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
	//proj[1].y *= -1;

	UploadUBOMatrix(model, view, proj);
}

VkPipelineVertexInputStateCreateInfo GP2Shader2D::CreateVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{ };

	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_AttributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &m_InputBinding;
	vertexInputInfo.pVertexAttributeDescriptions = m_AttributeDescriptions.data();
	return vertexInputInfo;
}
