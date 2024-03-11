#pragma once
#include "vulkanbase/VulkanUtil.h"
class GP2Shader;
class GP2GraphicsPipeline
{
public:
	GP2GraphicsPipeline() = default;

	~GP2GraphicsPipeline() = default;

	GP2GraphicsPipeline(const GP2GraphicsPipeline& other) = delete;
	GP2GraphicsPipeline(GP2GraphicsPipeline&& other) noexcept = delete;
	GP2GraphicsPipeline& operator=(const GP2GraphicsPipeline& other) = delete;
	GP2GraphicsPipeline& operator=(GP2GraphicsPipeline&& other) noexcept = delete;


	void Initialize(const VkDevice& device, const VkFormat& swapChainImageFormat, GP2Shader& shader);

	void Destroy(const VkDevice& device);

	VkRenderPass GetRenderPass() const;
	VkPipeline GetGraphicsPipeline() const;
	
private:
	void CreateRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat);
	void CreateGraphicsPipeline(const VkDevice& device, GP2Shader& shader);

	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;
	VkRenderPass m_RenderPass;

};
