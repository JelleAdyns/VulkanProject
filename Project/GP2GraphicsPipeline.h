#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "Vertex.h"
#include "OBJParser.h"
#include "GP2Shader.h"
#include "GP2Mesh.h"
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
		m_Shader.DestroyUniformObjects(device);
		vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
	}



	VkPipeline GetGraphicsPipeline() const { return m_GraphicsPipeline; }
	VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }


	void Initialize(const VulkanContext& vulkanContext, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);

	void UpdateMeshMatrix(const glm::mat4& model, size_t meshIndex);
	void UpdateUniformBuffer(uint32_t currentImage, glm::mat4 view, glm::mat4 projection);
	void Record(const GP2CommandBuffer& cmdBuffer, VkExtent2D vkExtent) const;

	void AddMesh(const std::string& objFile, const MeshContext& meshContext);
	void AddRectangle(float top, float left, float bottom, float right, const MeshContext& meshContext);
	void AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const MeshContext& meshContext);
	void AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const MeshContext& meshContext);
	void DestroyMeshes(const VkDevice& device);
private:

	VkPushConstantRange CreatePushConstantRange()
	{
		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // Stage the push constant is accessible from
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(MeshData); // Size of push constant block
		return pushConstantRange;
	}
	void Draw(const VkCommandBuffer& cmdBuffer) const;

	VkPipelineLayout m_PipelineLayout{};
	VkPipeline m_GraphicsPipeline{};
	VkRenderPass m_RenderPass{};

	std::vector<GP2Mesh<VertexType>> m_VecMeshes;
	GP2Shader<VertexType> m_Shader;

	void MakeRect(GP2Mesh<VertexType>& mesh, float top, float left, float bottom, float right) const;
	void CreateGraphicsPipeline(const VkDevice& device);
};



template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::Initialize(const VulkanContext& vulkanContext, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat)
{
	m_RenderPass = vulkanContext.renderPass;
	m_Shader.Init(vulkanContext);
	CreateGraphicsPipeline(vulkanContext.device);
}
template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::UpdateMeshMatrix(const glm::mat4& model, size_t meshIndex)
{
	m_VecMeshes[meshIndex].SetModelMatrix(MeshData{ model });
}
template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::UpdateUniformBuffer(uint32_t currentImage, glm::mat4 view, glm::mat4 projection)
{
	m_Shader.UploadUBOMatrix(view, projection);
}
template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::Record(const GP2CommandBuffer& cmdBuffer, VkExtent2D vkExtent) const
{
	vkCmdBindPipeline(cmdBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GetGraphicsPipeline());

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
	rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
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

	VkPushConstantRange pushConstantRange = CreatePushConstantRange();
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;

	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	pipelineLayoutInfo.pSetLayouts = &m_Shader.GetDescriptorSetLayout();

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
	pipelineInfo.layout = GetPipelineLayout();
	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	m_Shader.DestroyShaderModules(device);
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::AddMesh(const std::string& objFile, const MeshContext& meshContext)
{
	GP2Mesh<Vertex3D> mesh{};
	std::vector<Vertex3D> vertices{};
	std::vector<uint32_t> indices{};

	ParseOBJ(objFile, vertices, indices);

	for (auto& vertex : vertices)
	{
		vertex.color = glm::vec3{ 1.f,1.f,1.f };
		mesh.AddVertex(vertex);

	}
	for (const auto& index : indices) mesh.AddIndex(index);

	mesh.UploadBuffers(meshContext);

	m_VecMeshes.push_back(std::move(mesh));
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::AddRectangle(float top, float left, float bottom, float right, const MeshContext& meshContext)
{
	/*GP2Mesh<Vertex2D> rect;
	MakeRect(rect, top, left, bottom, right);
	rect.UploadBuffers(physicalDevice, device, commandPool, graphicsQueue);
	m_VecMeshes.push_back(std::move(rect));*/
	std::vector<Vertex3D> vertices =
	{
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint32_t> indices =
	{
		0, 1, 2, 2, 3, 0
	};

	GP2Mesh<Vertex3D> rect{};
	for (const auto& vertex : vertices)
		rect.AddVertex(vertex);
	for (const auto& index : indices)
		rect.AddIndex(index);

	rect.UploadBuffers(meshContext);
	m_VecMeshes.push_back(std::move(rect));


	vertices =
	{
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	indices = { 0, 1, 2, 2, 3, 0 };

	GP2Mesh<Vertex3D> rect2;
	for (const auto& vertex : vertices)
		rect2.AddVertex(vertex);
	for (const auto& index : indices)
		rect2.AddIndex(index);

	rect2.UploadBuffers(meshContext);
	m_VecMeshes.push_back(std::move(rect2));

}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const MeshContext& meshContext)
{

	assert((left < right) && "Left is greater than right");
	assert((top < bottom) && "Top is greater than bottom");
	assert((radiusX > 0 && radiusY > 0) && "Radius is less or equal than 0");

	//	    LAYOUT
	// 
	//	 ---4---5---
	//	/	|	|	\
	//	6---0---1---8
	//	|	|	|	|
	//	7---3---2---9
	//	\	|	|	/
	//	 ---10-11---

	//int currMaxIndex{-1};
	//GP2Mesh rect{};

	////MiddleRect
	//Vertex2D verticesMiddleRect[4]{};
	//verticesMiddleRect[0] = { glm::vec2{left + radiusX, top + radiusY},glm::vec3{0.0f,0.0f,1.0f} };
	//verticesMiddleRect[1] = { glm::vec2{right - radiusX, top + radiusY},glm::vec3{1.0f,0.0f,0.0f} };
	//verticesMiddleRect[2] = { glm::vec2{ right - radiusX, bottom - radiusY }, glm::vec3{1.0f,0.0f,0.0f} };
	//verticesMiddleRect[3] = { glm::vec2{ left + radiusX, bottom - radiusY },glm::vec3{0.0f,0.0f,1.0f} };

	//rect.AddVertex(verticesMiddleRect[0].pos, verticesMiddleRect[0].color);
	//rect.AddVertex(verticesMiddleRect[1].pos, verticesMiddleRect[1].color);
	//rect.AddVertex(verticesMiddleRect[2].pos, verticesMiddleRect[2].color);
	//rect.AddVertex(verticesMiddleRect[3].pos, verticesMiddleRect[3].color);
	//currMaxIndex += 4;

	//rect.AddIndex(0);
	//rect.AddIndex(1);
	//rect.AddIndex(2);

	//rect.AddIndex(0);
	//rect.AddIndex(2);
	//rect.AddIndex(3);

	////TopRect
	//Vertex2D verticesTopRect[2]{};
	//verticesTopRect[0] = { glm::vec2{left + radiusX, top},glm::vec3{0.0f,0.0f,1.0f} };
	//verticesTopRect[1] = { glm::vec2{right - radiusX, top},glm::vec3{1.0f,0.0f,0.0f} };

	//rect.AddVertex(verticesTopRect[0].pos, verticesTopRect[0].color);
	//rect.AddVertex(verticesTopRect[1].pos, verticesTopRect[1].color);
	//currMaxIndex += 2;

	//rect.AddIndex(4);
	//rect.AddIndex(5);
	//rect.AddIndex(1);

	//rect.AddIndex(4);
	//rect.AddIndex(1);
	//rect.AddIndex(0);

	////LeftRect
	//Vertex2D verticesLeftRect[2]{};
	//verticesLeftRect[0] = { glm::vec2{ left, top + radiusY },glm::vec3{0.0f,1.0f,0.0f} };
	//verticesLeftRect[1] = { glm::vec2{ left, bottom - radiusY },glm::vec3{0.0f,1.0f,0.0f} };

	//rect.AddVertex(verticesLeftRect[0].pos, verticesLeftRect[0].color);
	//rect.AddVertex(verticesLeftRect[1].pos, verticesLeftRect[1].color);
	//currMaxIndex += 2;

	//rect.AddIndex(6);
	//rect.AddIndex(0);
	//rect.AddIndex(3);

	//rect.AddIndex(6);
	//rect.AddIndex(3);
	//rect.AddIndex(7);

	////RightRect
	//Vertex2D verticesRightRect[2]{};
	//verticesRightRect[0] = { glm::vec2{ right, top + radiusY },glm::vec3{0.0f,1.0f,0.0f} };
	//verticesRightRect[1] = { glm::vec2{ right, bottom - radiusY },glm::vec3{0.0f,1.0f,0.0f } };

	//rect.AddVertex(verticesRightRect[0].pos, verticesRightRect[0].color);
	//rect.AddVertex(verticesRightRect[1].pos, verticesRightRect[1].color);
	//currMaxIndex += 2;

	//rect.AddIndex(1);
	//rect.AddIndex(8);
	//rect.AddIndex(9);

	//rect.AddIndex(1);
	//rect.AddIndex(9);
	//rect.AddIndex(2);

	////BottomRect
	//Vertex2D verticesBottomRect[2]{};
	//verticesBottomRect[0] = { glm::vec2{ left + radiusX, bottom },glm::vec3{0.0f,0.0f,1.0f} };
	//verticesBottomRect[1] = { glm::vec2{ right - radiusX, bottom },glm::vec3{1.0f,0.0f,0.0f } };

	//rect.AddVertex(verticesBottomRect[0].pos, verticesBottomRect[0].color);
	//rect.AddVertex(verticesBottomRect[1].pos, verticesBottomRect[1].color);
	//currMaxIndex += 2;

	//rect.AddIndex(3);
	//rect.AddIndex(2);
	//rect.AddIndex(11);

	//rect.AddIndex(3);
	//rect.AddIndex(11);
	//rect.AddIndex(10);



	////CORNERS
	//Vertex2D currEdgeVertex;
	//constexpr float pi = 3.14159265359f;
	//float radians = pi / 2 / numberOfSegmentsPerCorner;

	////TopLeftCorner
	//
	//rect.AddIndex(0);
	//rect.AddIndex(6);

	//currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	//for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	//{
	//	currEdgeVertex.pos.x = verticesMiddleRect[0].pos.x + radiusX * glm::cos(radians * i + pi);
	//	currEdgeVertex.pos.y = verticesMiddleRect[0].pos.y + radiusY * glm::sin(radians * i + pi);

	//	rect.AddVertex(currEdgeVertex.pos, currEdgeVertex.color);
	//	++currMaxIndex;

	//	//triangle
	//	rect.AddIndex(currMaxIndex);
	//	if (i > 1)
	//	{
	//		rect.AddIndex(0);
	//		rect.AddIndex(currMaxIndex-1);
	//	}
	//}

	//rect.AddIndex(0);
	//rect.AddIndex(currMaxIndex);
	//rect.AddIndex(4);


	////TopRightCorner
	//rect.AddIndex(1);
	//rect.AddIndex(5);

	//currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	//for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	//{
	//	currEdgeVertex.pos.x = verticesMiddleRect[1].pos.x + radiusX * glm::cos(radians * i - pi/2);
	//	currEdgeVertex.pos.y = verticesMiddleRect[1].pos.y + radiusY * glm::sin(radians * i - pi/2);

	//	rect.AddVertex(currEdgeVertex.pos, currEdgeVertex.color);
	//	++currMaxIndex;

	//	//triangle
	//	rect.AddIndex(currMaxIndex);
	//	if (i > 1)
	//	{
	//		rect.AddIndex(1);
	//		rect.AddIndex(currMaxIndex - 1);
	//	}
	//}

	//rect.AddIndex(1);
	//rect.AddIndex(currMaxIndex);
	//rect.AddIndex(8);


	////BottomLeftCorner
	//rect.AddIndex(3);
	//rect.AddIndex(10);

	//currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	//for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	//{
	//	currEdgeVertex.pos.x = verticesMiddleRect[3].pos.x + radiusX * glm::cos(radians * i + pi / 2);
	//	currEdgeVertex.pos.y = verticesMiddleRect[3].pos.y + radiusY * glm::sin(radians * i + pi / 2);

	//	rect.AddVertex(currEdgeVertex.pos, currEdgeVertex.color);
	//	++currMaxIndex;

	//	//triangle
	//	rect.AddIndex(currMaxIndex);
	//	if (i > 1)
	//	{
	//		rect.AddIndex(3);
	//		rect.AddIndex(currMaxIndex - 1);
	//	}
	//}

	//rect.AddIndex(3);
	//rect.AddIndex(currMaxIndex);
	//rect.AddIndex(7);


	////BottomRightCorner
	//rect.AddIndex(2);
	//rect.AddIndex(9);

	//currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	//for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	//{
	//	currEdgeVertex.pos.x = verticesMiddleRect[2].pos.x + radiusX * glm::cos(radians * i);
	//	currEdgeVertex.pos.y = verticesMiddleRect[2].pos.y + radiusY * glm::sin(radians * i);

	//	rect.AddVertex(currEdgeVertex.pos, currEdgeVertex.color);
	//	++currMaxIndex;

	//	//triangle
	//	rect.AddIndex(currMaxIndex);
	//	if (i > 1)
	//	{
	//		rect.AddIndex(2);
	//		rect.AddIndex(currMaxIndex - 1);
	//	}
	//}

	//rect.AddIndex(2);
	//rect.AddIndex(currMaxIndex);
	//rect.AddIndex(11);
	//

	//rect.Initialize(physicalDevice, device, commandPool, graphicsQueue);
	//m_VecMeshes.push_back(std::move(rect));
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const MeshContext& meshContext)
{

	/*assert((radiusX > 0 && radiusY > 0));
	constexpr float pi = 3.14159265359f;

	float radians = pi * 2 / numberOfSegments;

	GP2Mesh oval;

	Vertex2D center  {glm::vec2{centerX, centerY}, glm::vec3{0.0f,0.0f,1.0f}};
	Vertex2D currEdgeVertex { {}, glm::vec3{0.0f,1.0f,0.0f} };

	oval.AddVertex(center.pos, center.color);
	for (int i = 1; i <= numberOfSegments; i++)
	{
		currEdgeVertex.pos.x = centerX + radiusX * glm::cos(radians * i);
		currEdgeVertex.pos.y = centerY + radiusY * glm::sin(radians * i);

		oval.AddVertex(currEdgeVertex.pos, currEdgeVertex.color);

		oval.AddIndex(0);
		oval.AddIndex(i);
		if (i == numberOfSegments) oval.AddIndex(1);
		else oval.AddIndex(i+1);
	}

	oval.Initialize(physicalDevice, device, commandPool, graphicsQueue);
	m_VecMeshes.push_back(std::move(oval));*/
}

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::Draw(const VkCommandBuffer& cmdBuffer) const
{
	m_Shader.GetDescriptorPool()->BindDescriptorSet(cmdBuffer, GetPipelineLayout(), 0);
	for (const auto& mesh : m_VecMeshes)
	{
		mesh.Draw(GetPipelineLayout(), cmdBuffer);
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

template <typename VertexType>
void GP2GraphicsPipeline<VertexType>::MakeRect(GP2Mesh<VertexType>& mesh, float top, float left, float bottom, float right) const
{
	assert((left < right) && "Left is greater than right");
	assert((top < bottom) && "Top is greater than bottom");
	constexpr int nrOfVertices{ 4 };
	constexpr int nrOfIndices{ 6 };

	Vertex3D vertices[nrOfVertices]{ {glm::vec3{left, top, 0}, glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec3{right, top, 0}, glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec3{right, bottom, 0}, glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec3{left, bottom, 0}, glm::vec3{0.0f,0.0f,1.0f}} };

	int indices[nrOfIndices]{ 0,1,2,0,2,3 };

	for (int i = 0; i < nrOfVertices; i++) mesh.AddVertex(vertices[i]);
	for (int i = 0; i < nrOfIndices; i++) mesh.AddIndex(indices[i]);
}
