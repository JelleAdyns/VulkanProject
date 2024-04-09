#include "GP2GraphicsPipeline3D.h"
#include "OBJParser.h"

void GP2GraphicsPipeline3D::Initialize(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat, GP2Shader3D& shader)
{
	CreateRenderPass(device, swapChainImageFormat, depthFormat);
	CreateGraphicsPipeline(device, shader);
}

void GP2GraphicsPipeline3D::CreateRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat) 
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = depthFormat;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkRenderPass renderPass{};

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create render pass!");
	}

	SetRenderPass(renderPass);
}

void GP2GraphicsPipeline3D::CreateGraphicsPipeline(const VkDevice& device, GP2Shader3D& shader) 
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

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	//pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pSetLayouts = &shader.GetDescriptorSetLayout();



	VkPipelineLayout pipelineLayout{};
	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}
	SetPipelineLayout(pipelineLayout);



	VkGraphicsPipelineCreateInfo pipelineInfo{};

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

	pipelineInfo.stageCount = (uint32_t)shader.GetShaderStageInfos().size();
	pipelineInfo.pStages = shader.GetShaderStageInfos().data();
	pipelineInfo.pVertexInputState = &shader.CreateVertexInputStateInfo();
	pipelineInfo.pInputAssemblyState = &shader.CreateInputAssemblyStateInfo();

	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = GetPipelineLayout();
	pipelineInfo.renderPass = GetRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;


	VkPipeline graphicsPipeline{};
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create graphics pipeline!");
	}
	SetGraphicsPipeline(graphicsPipeline);

	shader.DestroyShaderModules(device);
}
void GP2GraphicsPipeline3D::AddMesh(const std::string& objFile, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
{
	GP2Mesh<Vertex3D> mesh{};
	std::vector<Vertex3D> vertices{};
	std::vector<uint32_t> indices{};

	ParseOBJ(objFile, vertices, indices);

	for (auto& vertex : vertices)
	{
		vertex.color = glm::vec3{1.f,1.f,1.f};
		mesh.AddVertex(vertex);

	}
	for (const auto& index : indices) mesh.AddIndex(index);

	mesh.UploadBuffers(physicalDevice, device, commandPool, graphicsQueue);

	m_VecMeshes.push_back(std::move(mesh));
}

void GP2GraphicsPipeline3D::AddRectangle(float top, float left, float bottom, float right,
	const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
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
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint32_t> indices =
	{
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
	};

	GP2Mesh<Vertex3D> rect{};
	for (const auto& vertex : vertices)
		rect.AddVertex(vertex);
	for (const auto& index : indices)
		rect.AddIndex(index);

	rect.UploadBuffers(physicalDevice, device, commandPool, graphicsQueue);
	m_VecMeshes.push_back(std::move(rect));




	vertices =
	{
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	indices = { 4, 5, 6, 6, 7, 4 };

	GP2Mesh<Vertex3D> rect2;
	for (const auto& vertex : vertices)
		rect2.AddVertex(vertex);
	for (const auto& index : indices)
		rect2.AddIndex(index);

	rect2.UploadBuffers(physicalDevice, device, commandPool, graphicsQueue);
	m_VecMeshes.push_back(std::move(rect2));

}

void GP2GraphicsPipeline3D::AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner,
	const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
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

void GP2GraphicsPipeline3D::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments,
	const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
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

void GP2GraphicsPipeline3D::Draw(const VkCommandBuffer& cmdBuffer) const
{
	for (const auto& mesh : m_VecMeshes)
	{
		mesh.Draw(cmdBuffer);
	}
}

void GP2GraphicsPipeline3D::DestroyMeshes(const VkDevice& device)
{
	for (auto& mesh : m_VecMeshes)
	{
		mesh.DestroyMesh(device);
	}
}

void GP2GraphicsPipeline3D::MakeRect(GP2Mesh<Vertex3D>& mesh, float top, float left, float bottom, float right) const
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
