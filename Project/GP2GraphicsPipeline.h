#pragma once
#include "vulkanbase/VulkanUtil.h"
#include <array>

class GP2GraphicsPipeline
{
public:

	virtual ~GP2GraphicsPipeline() = default;

	GP2GraphicsPipeline(const GP2GraphicsPipeline& other) = delete;
	GP2GraphicsPipeline(GP2GraphicsPipeline&& other) noexcept = delete;
	GP2GraphicsPipeline& operator=(const GP2GraphicsPipeline& other) = delete;
	GP2GraphicsPipeline& operator=(GP2GraphicsPipeline&& other) noexcept = delete;


	void Destroy(const VkDevice& device)
	{
		vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(device, m_RenderPass, nullptr);
	}


	VkRenderPass GetRenderPass() const { return m_RenderPass; }
	VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }
	VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
	
protected:
	GP2GraphicsPipeline() = default;


	void SetRenderPass(VkRenderPass renderPass) { m_RenderPass = renderPass; }
	void SetGraphicsPipeline(VkPipeline graphicsPipeline) { m_GraphicsPipeline = graphicsPipeline; }
	void SetPipelineLayout(VkPipelineLayout pipelineLayout) { m_PipelineLayout = pipelineLayout; }
private:

	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;
	VkRenderPass m_RenderPass;

};
