#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
#include "OBJParser.h"
#include "GP2Shader.h"
#include "GP2Mesh.h"
#include "GP2Texture.h"
#include <array>


template< typename VertexType>
class GP2GraphicsPipeline
{
public:

	GP2GraphicsPipeline(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) :
		m_VecMeshes{},
		m_Shader{ vertexShaderFile,fragmentShaderFile }
	{}
	~GP2GraphicsPipeline() = default;

	GP2GraphicsPipeline(const GP2GraphicsPipeline& other) = delete;
	GP2GraphicsPipeline(GP2GraphicsPipeline&& other) noexcept = delete;
	GP2GraphicsPipeline& operator=(const GP2GraphicsPipeline& other) = delete;
	GP2GraphicsPipeline& operator=(GP2GraphicsPipeline&& other) noexcept = delete;

	void Destroy(const VkDevice& device)
	{
		m_pDescriptorPool->DestroyPool(device);

		vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
	}

	VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }
	VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

	void DestroyMeshes(const VkDevice& device);
	void Initialize(const VulkanContext& vulkanContext, const MeshContext& meshContext, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);


	void AddGP2Mesh(const GP2Mesh<VertexType>& mesh);
	void UpdateMeshMatrix(const glm::mat4& model, size_t meshIndex);
	void SetMaterial(GP2Material* pMaterial, int meshIndex);
	void UpdateUniformBuffer(const glm::mat4& view, const glm::mat4& projection);
	void Record(const GP2CommandBuffer& cmdBuffer, VkExtent2D vkExtent) const;

private:

	void Draw(const VkCommandBuffer& cmdBuffer) const;

	VkPipelineLayout m_PipelineLayout{};
	VkPipeline m_GraphicsPipeline{};
	VkRenderPass m_RenderPass{};

	std::vector<GP2Mesh<VertexType>> m_VecMeshes;
	GP2Shader<VertexType> m_Shader;
	std::unique_ptr<GP2DescriptorPool<ViewProjection, VertexType>> m_pDescriptorPool;

	void CreateGraphicsPipeline(const VkDevice& device);
	VkPushConstantRange CreatePushConstantRange(VkShaderStageFlagBits shaderStage, uint32_t offset, uint32_t sizeOfObject);
};



template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::Initialize(const VulkanContext& vulkanContext, const MeshContext& meshContext, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat)
{
	m_RenderPass = vulkanContext.renderPass;
	m_Shader.Init(vulkanContext);

	m_pDescriptorPool = std::make_unique<GP2DescriptorPool<ViewProjection, VertexType>>(vulkanContext, m_VecMeshes);

	CreateGraphicsPipeline(vulkanContext.device);

}

template<typename VertexType>
void GP2GraphicsPipeline<VertexType>::AddGP2Mesh(const GP2Mesh<VertexType>& mesh)
{	
	m_VecMeshes.push_back(mesh);
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::UpdateMeshMatrix(const glm::mat4& model, size_t meshIndex)
{
	m_VecMeshes[meshIndex].SetModelMatrix(MeshData{ model });
}
template<typename VertexType>
inline void GP2GraphicsPipeline<VertexType>::SetMaterial(GP2Material* pMaterial, int meshIndex)
{
	m_VecMeshes[meshIndex].SetMaterial(pMaterial);
}
template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::UpdateUniformBuffer(const glm::mat4& view, const glm::mat4& projection)
{
	ViewProjection viewProj{};

	viewProj.view = view;
	viewProj.proj = projection;

	m_pDescriptorPool->SetUBO(viewProj, 0);
}
template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::Record(const GP2CommandBuffer& cmdBuffer, VkExtent2D vkExtent) const
{
	vkCmdBindPipeline(cmdBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vkExtent.width;
	viewport.height = (float)vkExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(cmdBuffer.GetVkCommandBuffer(), 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = vkExtent;
	vkCmdSetScissor(cmdBuffer.GetVkCommandBuffer(), 0, 1, &scissor);

	Draw(cmdBuffer.GetVkCommandBuffer());
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::CreateGraphicsPipeline(const VkDevice& device)
{
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPushConstantRange pushConstantRangeMesh = CreatePushConstantRange(VK_SHADER_STAGE_VERTEX_BIT,0, sizeof(MeshData));
	//VkPushConstantRange pushConstantRangeCamera = CreatePushConstantRange(VK_SHADER_STAGE_FRAGMENT_BIT,1, sizeof(m_CameraPos));
	std::vector< VkPushConstantRange> ranges{ pushConstantRangeMesh  };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;

	pipelineLayoutInfo.pushConstantRangeCount =static_cast<uint32_t>(ranges.size());
	pipelineLayoutInfo.pPushConstantRanges = ranges.data();

	pipelineLayoutInfo.pSetLayouts = &m_pDescriptorPool->GetDescriptorSetLayout();

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}


	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	pipelineInfo.stageCount = (uint32_t)m_Shader.GetShaderStageInfos().size();
	pipelineInfo.pStages = m_Shader.GetShaderStageInfos().data();
	pipelineInfo.pVertexInputState = &m_Shader.CreateVertexInputStateInfo();
	pipelineInfo.pInputAssemblyState = &m_Shader.CreateInputAssemblyStateInfo();

	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = m_PipelineLayout;
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	m_Shader.DestroyShaderModules(device);
}

template<typename VertexType>
VkPushConstantRange GP2GraphicsPipeline<VertexType>::CreatePushConstantRange(VkShaderStageFlagBits shaderStage, uint32_t offset, uint32_t sizeOfObject)
{
	VkPushConstantRange pushConstantRange = {};
	pushConstantRange.stageFlags = shaderStage; // Stage the push constant is accessible from
	pushConstantRange.offset = offset;
	pushConstantRange.size = sizeOfObject; // Size of push constant block
	return pushConstantRange;
}



template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::Draw(const VkCommandBuffer& cmdBuffer) const
{
	for (size_t i = 0; i < m_VecMeshes.size(); i++)
	{
		m_pDescriptorPool->BindDescriptorSet(cmdBuffer, GetPipelineLayout(), i);
		m_VecMeshes[i].Draw(GetPipelineLayout(), cmdBuffer);
	}
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::DestroyMeshes(const VkDevice& device)
{
	for (auto& mesh : m_VecMeshes)
	{
		mesh.DestroyMesh(device);
	}
}