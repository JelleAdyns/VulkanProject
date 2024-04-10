#include "GP2GraphicsPipeline2D.h"
//#include <glm/gtc/matrix_transform.hpp>
//#include <chrono>
//
//
//void GP2GraphicsPipeline2D::Initialize(const VulkanContext& vulkanContext, const VkFormat& swapChainImageFormat)
//{
//	CreateRenderPass(vulkanContext, swapChainImageFormat);
//	m_Shader.Init(vulkanContext);
//	CreateGraphicsPipeline(vulkanContext.device);
//}
//
//void GP2GraphicsPipeline2D::UpdateUniformBuffer(uint32_t currentImage, glm::mat4 view, glm::mat4 projection)
//{
//	static auto startTime = std::chrono::high_resolution_clock::now();
//
//	auto currentTime = std::chrono::high_resolution_clock::now();
//	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//	auto model = glm::rotate(glm::mat4(1.0f), time * glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//	//auto view = glm::mat4(1.0f);// glm::lookAt(glm::vec3(0, 0, 6), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//	//auto proj = glm::mat4(1.0f);//glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
//	//proj[1].y *= -1;
//	
//	m_Shader.UploadUBOMatrix(model, view, projection);
//}
//
//void GP2GraphicsPipeline2D::Record(const GP2CommandBuffer& cmdBuffer, VkExtent2D vkExtent) const
//{
//	vkCmdBindPipeline(cmdBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, GetGraphicsPipeline());
//
//	VkViewport viewport{};
//	viewport.x = 0.0f;
//	viewport.y = 0.0f;
//	viewport.width = (float)vkExtent.width;
//	viewport.height = (float)vkExtent.height;
//	viewport.minDepth = 0.0f;
//	viewport.maxDepth = 1.0f;
//	vkCmdSetViewport(cmdBuffer.GetVkCommandBuffer(), 0, 1, &viewport);
//
//	VkRect2D scissor{};
//	scissor.offset = { 0, 0 };
//	scissor.extent = vkExtent;
//	vkCmdSetScissor(cmdBuffer.GetVkCommandBuffer(), 0, 1, &scissor);
//
//	Draw(cmdBuffer.GetVkCommandBuffer());
//}
//
//void GP2GraphicsPipeline2D::CreateRenderPass(const VulkanContext& vulkanContext, const VkFormat& swapChainImageFormat)
//{
//	VkAttachmentDescription colorAttachment{};
//	colorAttachment.format = swapChainImageFormat;
//	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
//	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//	VkAttachmentReference colorAttachmentRef{};
//	colorAttachmentRef.attachment = 0;
//	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//	VkSubpassDescription subpass{};
//	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//	subpass.colorAttachmentCount = 1;
//	subpass.pColorAttachments = &colorAttachmentRef;
//
//	VkRenderPassCreateInfo renderPassInfo{};
//	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//	renderPassInfo.attachmentCount = 1;
//	renderPassInfo.pAttachments = &colorAttachment;
//	renderPassInfo.subpassCount = 1;
//	renderPassInfo.pSubpasses = &subpass;
//
//	VkRenderPass renderPass{};
//
//	if (vkCreateRenderPass(vulkanContext.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create render pass!");
//	}
//
//	SetRenderPass(renderPass);
//}
//
//void GP2GraphicsPipeline2D::CreateGraphicsPipeline(const VkDevice& device) 
//{
//	VkPipelineViewportStateCreateInfo viewportState{};
//	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//	viewportState.viewportCount = 1;
//	viewportState.scissorCount = 1;
//
//	VkPipelineRasterizationStateCreateInfo rasterizer{};
//	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//	rasterizer.depthClampEnable = VK_FALSE;
//	rasterizer.rasterizerDiscardEnable = VK_FALSE;
//	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//	rasterizer.lineWidth = 1.0f;
//	rasterizer.cullMode = VK_CULL_MODE_NONE;
//	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
//	rasterizer.depthBiasEnable = VK_FALSE;
//
//	VkPipelineMultisampleStateCreateInfo multisampling{};
//	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//	multisampling.sampleShadingEnable = VK_FALSE;
//	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
//	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//	colorBlendAttachment.blendEnable = VK_FALSE;
//
//	VkPipelineColorBlendStateCreateInfo colorBlending{};
//	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//	colorBlending.logicOpEnable = VK_FALSE;
//	colorBlending.logicOp = VK_LOGIC_OP_COPY;
//	colorBlending.attachmentCount = 1;
//	colorBlending.pAttachments = &colorBlendAttachment;
//	colorBlending.blendConstants[0] = 0.0f;
//	colorBlending.blendConstants[1] = 0.0f;
//	colorBlending.blendConstants[2] = 0.0f;
//	colorBlending.blendConstants[3] = 0.0f;
//
//	std::vector<VkDynamicState> dynamicStates = 
//	{
//		VK_DYNAMIC_STATE_VIEWPORT,
//		VK_DYNAMIC_STATE_SCISSOR
//	};
//	VkPipelineDynamicStateCreateInfo dynamicState{};
//	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
//	dynamicState.pDynamicStates = dynamicStates.data();
//
//	VkPushConstantRange pushConstantRange = CreatePushConstantRange();
//	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
//	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//	pipelineLayoutInfo.setLayoutCount = 1;
//
//	pipelineLayoutInfo.pushConstantRangeCount = 1;
//	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
//
//	pipelineLayoutInfo.pSetLayouts = &m_Shader.GetDescriptorSetLayout();
//
//	VkPipelineLayout pipelineLayout{};
//	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
//	{
//		throw std::runtime_error("failed to create pipeline layout!");
//	}
//	SetPipelineLayout(pipelineLayout);
//	
//	VkGraphicsPipelineCreateInfo pipelineInfo{};
//
//	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//
//
//	pipelineInfo.stageCount = (uint32_t)m_Shader.GetShaderStageInfos().size();
//	pipelineInfo.pStages = m_Shader.GetShaderStageInfos().data();
//	pipelineInfo.pVertexInputState = &m_Shader.CreateVertexInputStateInfo();
//	pipelineInfo.pInputAssemblyState = &m_Shader.CreateInputAssemblyStateInfo();
//
//
//	pipelineInfo.pViewportState = &viewportState;
//	pipelineInfo.pRasterizationState = &rasterizer;
//	pipelineInfo.pMultisampleState = &multisampling;
//	pipelineInfo.pColorBlendState = &colorBlending;
//	pipelineInfo.pDynamicState = &dynamicState;
//	pipelineInfo.layout = GetPipelineLayout();
//	pipelineInfo.renderPass = GetRenderPass();
//	pipelineInfo.subpass = 0;
//	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
//
//	VkPipeline pipeline{};
//	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS)
//	{
//		throw std::runtime_error("failed to create graphics pipeline!");
//	}
//	SetGraphicsPipeline(pipeline);
//
//	m_Shader.DestroyShaderModules(device);
//}
//
//
//void GP2GraphicsPipeline2D::AddMesh(const GP2Mesh<Vertex2D>& gp2Mesh)
//{
//	//m_VecMeshes.push_back(gp2Mesh);
//}
//
//void GP2GraphicsPipeline2D::AddRectangle(float top, float left, float bottom, float right, const MeshContext& meshContext)
//{
//	GP2Mesh<Vertex2D> rect;
//	MakeRect(rect, top, left, bottom, right);
//	rect.UploadBuffers(meshContext);
//	m_VecMeshes.push_back(std::move(rect));
//
//}
//
//void GP2GraphicsPipeline2D::AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const MeshContext& meshContext)
//{
//
//	assert((left < right) && "Left is greater than right");
//	assert((top < bottom) && "Top is greater than bottom");
//	assert((radiusX > 0 && radiusY > 0) && "Radius is less or equal than 0");
//
//	//	    LAYOUT
//	// 
//	//	 ---4---5---
//	//	/	|	|	\
//	//	6---0---1---8
//	//	|	|	|	|
//	//	7---3---2---9
//	//	\	|	|	/
//	//	 ---10-11---
//
//	int currMaxIndex{ -1 };
//	GP2Mesh<Vertex2D> rect{};
//
//	//MiddleRect
//	Vertex2D verticesMiddleRect[4]{};
//	verticesMiddleRect[0] = { glm::vec2{left + radiusX, top + radiusY},glm::vec3{0.0f,0.0f,1.0f} };
//	verticesMiddleRect[1] = { glm::vec2{right - radiusX, top + radiusY},glm::vec3{1.0f,0.0f,0.0f} };
//	verticesMiddleRect[2] = { glm::vec2{ right - radiusX, bottom - radiusY }, glm::vec3{1.0f,0.0f,0.0f} };
//	verticesMiddleRect[3] = { glm::vec2{ left + radiusX, bottom - radiusY },glm::vec3{0.0f,0.0f,1.0f} };
//
//	rect.AddVertex(verticesMiddleRect[0]);
//	rect.AddVertex(verticesMiddleRect[1]);
//	rect.AddVertex(verticesMiddleRect[2]);
//	rect.AddVertex(verticesMiddleRect[3]);
//	currMaxIndex += 4;
//
//	rect.AddIndex(0);
//	rect.AddIndex(1);
//	rect.AddIndex(2);
//
//	rect.AddIndex(0);
//	rect.AddIndex(2);
//	rect.AddIndex(3);
//
//	//TopRect
//	Vertex2D verticesTopRect[2]{};
//	verticesTopRect[0] = { glm::vec2{left + radiusX, top},glm::vec3{0.0f,0.0f,1.0f} };
//	verticesTopRect[1] = { glm::vec2{right - radiusX, top},glm::vec3{1.0f,0.0f,0.0f} };
//
//	rect.AddVertex(verticesTopRect[0]);
//	rect.AddVertex(verticesTopRect[1]);
//	currMaxIndex += 2;
//
//	rect.AddIndex(4);
//	rect.AddIndex(5);
//	rect.AddIndex(1);
//
//	rect.AddIndex(4);
//	rect.AddIndex(1);
//	rect.AddIndex(0);
//
//	//LeftRect
//	Vertex2D verticesLeftRect[2]{};
//	verticesLeftRect[0] = { glm::vec2{ left, top + radiusY },glm::vec3{0.0f,1.0f,0.0f} };
//	verticesLeftRect[1] = { glm::vec2{ left, bottom - radiusY },glm::vec3{0.0f,1.0f,0.0f} };
//
//	rect.AddVertex(verticesLeftRect[0]);
//	rect.AddVertex(verticesLeftRect[1]);
//	currMaxIndex += 2;
//
//	rect.AddIndex(6);
//	rect.AddIndex(0);
//	rect.AddIndex(3);
//
//	rect.AddIndex(6);
//	rect.AddIndex(3);
//	rect.AddIndex(7);
//
//	//RightRect
//	Vertex2D verticesRightRect[2]{};
//	verticesRightRect[0] = { glm::vec2{ right, top + radiusY },glm::vec3{0.0f,1.0f,0.0f} };
//	verticesRightRect[1] = { glm::vec2{ right, bottom - radiusY },glm::vec3{0.0f,1.0f,0.0f } };
//
//	rect.AddVertex(verticesRightRect[0]);
//	rect.AddVertex(verticesRightRect[1]);
//	currMaxIndex += 2;
//
//	rect.AddIndex(1);
//	rect.AddIndex(8);
//	rect.AddIndex(9);
//
//	rect.AddIndex(1);
//	rect.AddIndex(9);
//	rect.AddIndex(2);
//
//	//BottomRect
//	Vertex2D verticesBottomRect[2]{};
//	verticesBottomRect[0] = { glm::vec2{ left + radiusX, bottom },glm::vec3{0.0f,0.0f,1.0f} };
//	verticesBottomRect[1] = { glm::vec2{ right - radiusX, bottom },glm::vec3{1.0f,0.0f,0.0f } };
//
//	rect.AddVertex(verticesBottomRect[0]);
//	rect.AddVertex(verticesBottomRect[1]);
//	currMaxIndex += 2;
//
//	rect.AddIndex(3);
//	rect.AddIndex(2);
//	rect.AddIndex(11);
//
//	rect.AddIndex(3);
//	rect.AddIndex(11);
//	rect.AddIndex(10);
//
//
//
//	//CORNERS
//	Vertex2D currEdgeVertex;
//	constexpr float pi = 3.14159265359f;
//	float radians = pi / 2 / numberOfSegmentsPerCorner;
//
//	//TopLeftCorner
//
//	rect.AddIndex(0);
//	rect.AddIndex(6);
//
//	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };
//
//	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
//	{
//		currEdgeVertex.pos.x = verticesMiddleRect[0].pos.x + radiusX * glm::cos(radians * i + pi);
//		currEdgeVertex.pos.y = verticesMiddleRect[0].pos.y + radiusY * glm::sin(radians * i + pi);
//
//		rect.AddVertex(currEdgeVertex);
//		++currMaxIndex;
//
//		//triangle
//		rect.AddIndex(currMaxIndex);
//		if (i > 1)
//		{
//			rect.AddIndex(0);
//			rect.AddIndex(currMaxIndex - 1);
//		}
//	}
//
//	rect.AddIndex(0);
//	rect.AddIndex(currMaxIndex);
//	rect.AddIndex(4);
//
//
//	//TopRightCorner
//	rect.AddIndex(1);
//	rect.AddIndex(5);
//
//	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };
//
//	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
//	{
//		currEdgeVertex.pos.x = verticesMiddleRect[1].pos.x + radiusX * glm::cos(radians * i - pi / 2);
//		currEdgeVertex.pos.y = verticesMiddleRect[1].pos.y + radiusY * glm::sin(radians * i - pi / 2);
//
//		rect.AddVertex(currEdgeVertex);
//		++currMaxIndex;
//
//		//triangle
//		rect.AddIndex(currMaxIndex);
//		if (i > 1)
//		{
//			rect.AddIndex(1);
//			rect.AddIndex(currMaxIndex - 1);
//		}
//	}
//
//	rect.AddIndex(1);
//	rect.AddIndex(currMaxIndex);
//	rect.AddIndex(8);
//
//
//	//BottomLeftCorner
//	rect.AddIndex(3);
//	rect.AddIndex(10);
//
//	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };
//
//	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
//	{
//		currEdgeVertex.pos.x = verticesMiddleRect[3].pos.x + radiusX * glm::cos(radians * i + pi / 2);
//		currEdgeVertex.pos.y = verticesMiddleRect[3].pos.y + radiusY * glm::sin(radians * i + pi / 2);
//
//		rect.AddVertex(currEdgeVertex);
//		++currMaxIndex;
//
//		//triangle
//		rect.AddIndex(currMaxIndex);
//		if (i > 1)
//		{
//			rect.AddIndex(3);
//			rect.AddIndex(currMaxIndex - 1);
//		}
//	}
//
//	rect.AddIndex(3);
//	rect.AddIndex(currMaxIndex);
//	rect.AddIndex(7);
//
//
//	//BottomRightCorner
//	rect.AddIndex(2);
//	rect.AddIndex(9);
//
//	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };
//
//	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
//	{
//		currEdgeVertex.pos.x = verticesMiddleRect[2].pos.x + radiusX * glm::cos(radians * i);
//		currEdgeVertex.pos.y = verticesMiddleRect[2].pos.y + radiusY * glm::sin(radians * i);
//
//		rect.AddVertex(currEdgeVertex);
//		++currMaxIndex;
//
//		//triangle
//		rect.AddIndex(currMaxIndex);
//		if (i > 1)
//		{
//			rect.AddIndex(2);
//			rect.AddIndex(currMaxIndex - 1);
//		}
//	}
//
//	rect.AddIndex(2);
//	rect.AddIndex(currMaxIndex);
//	rect.AddIndex(11);
//
//
//	rect.UploadBuffers(meshContext);
//	m_VecMeshes.push_back(std::move(rect));
//}
//
//void GP2GraphicsPipeline2D::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments,
//	const MeshContext& meshContext)
//{
//
//	assert((radiusX > 0 && radiusY > 0));
//	constexpr float pi = 3.14159265359f;
//
//	float radians = pi * 2 / numberOfSegments;
//
//	GP2Mesh<Vertex2D> oval;
//
//	Vertex2D center  {glm::vec2{centerX, centerY}, glm::vec3{0.0f,0.0f,1.0f}};
//	Vertex2D currEdgeVertex { {}, glm::vec3{0.0f,1.0f,0.0f} };
//
//	oval.AddVertex(center);
//	for (int i = 1; i <= numberOfSegments; i++)
//	{
//		currEdgeVertex.pos.x = centerX + radiusX * glm::cos(radians * i);
//		currEdgeVertex.pos.y = centerY + radiusY * glm::sin(radians * i);
//
//		oval.AddVertex(currEdgeVertex);
//
//		oval.AddIndex(0);
//		oval.AddIndex(i);
//		if (i == numberOfSegments) oval.AddIndex(1);
//		else oval.AddIndex(i+1);
//	}
//
//	oval.UploadBuffers(meshContext);
//	m_VecMeshes.push_back(std::move(oval));
//}
//
//void GP2GraphicsPipeline2D::Draw(const VkCommandBuffer& cmdBuffer) const
//{
//	m_Shader.GetDescriptorPool()->BindDescriptorSet(cmdBuffer, GetPipelineLayout(), 0);
//	for (const auto& mesh : m_VecMeshes)
//	{
//		mesh.Draw(GetPipelineLayout(), cmdBuffer);
//	}
//}
//
//void GP2GraphicsPipeline2D::DestroyMeshes(const VkDevice& device)
//{
//	for (auto& mesh : m_VecMeshes)
//	{
//		mesh.DestroyMesh(device);
//	}
//}
//
//void GP2GraphicsPipeline2D::MakeRect(GP2Mesh<Vertex2D>& mesh, float top, float left, float bottom, float right) const
//{
//	assert((left < right) && "Left is greater than right");
//	assert((top < bottom) && "Top is greater than bottom");
//	constexpr int nrOfVertices{ 4 };
//	constexpr int nrOfIndices{ 6 };
//
//	Vertex2D vertices[nrOfVertices]{ {glm::vec2{left, top},glm::vec3{1.0f,0.0f,0.0f}},
//						{glm::vec2{right, top},glm::vec3{0.0f,1.0f,0.0f}},
//						{glm::vec2{right, bottom},glm::vec3{1.0f,0.0f,0.0f}},
//						{glm::vec2{left, bottom},glm::vec3{0.0f,0.0f,1.0f}} };
//
//	int indices[nrOfIndices]{ 0,1,2,0,2,3 };
//
//	for (int i = 0; i < nrOfVertices; i++) mesh.AddVertex(vertices[i]);
//	for (int i = 0; i < nrOfIndices; i++) mesh.AddIndex(indices[i]);
//}
