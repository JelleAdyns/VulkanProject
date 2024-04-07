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


	void Initialize(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat, GP2Shader& shader);

	void Destroy(const VkDevice& device);

	VkRenderPass GetRenderPass() const;
	VkPipeline GetGraphicsPipeline() const;
	VkPipelineLayout GetPipelineLayout() const;
	
protected:

	virtual void CreateRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);
	virtual void CreateGraphicsPipeline(const VkDevice& device, GP2Shader& shader);
private:

	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;
	VkRenderPass m_RenderPass;

};
