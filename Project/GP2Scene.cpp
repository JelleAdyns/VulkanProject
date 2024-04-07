#include "GP2Scene.h"
#include "GP2Mesh.h"
#include "Vertex.h"
#include <assert.h>

void GP2Scene::AddMesh(const GP2Mesh& gp2Mesh)
{
	//m_VecMeshes.push_back(gp2Mesh);
}

void GP2Scene::AddRectangle(float top, float left, float bottom, float right, 
	const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
{
	
	//MakeRect(rect, top, left, bottom, right);
	std::vector<Vertex3D> vertices = {
		{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4
	};
	
	GP2Mesh rect{};
	for (const auto& vertex : vertices)
		rect.AddVertex(vertex.pos, vertex.color);
	for (const auto& index : indices)
		rect.AddIndex(index);
	
	rect.Initialize(physicalDevice, device, commandPool, graphicsQueue);
	m_VecMeshes.push_back(std::move(rect));




	vertices = {
		{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
		{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}} };
	indices = { 4, 5, 6, 6, 7, 4 };

	GP2Mesh rect2;
	for (const auto& vertex : vertices)
		rect2.AddVertex(vertex.pos, vertex.color);
	for (const auto& index : indices)
		rect2.AddIndex(index);

	rect2.Initialize(physicalDevice, device, commandPool, graphicsQueue);
	m_VecMeshes.push_back(std::move(rect2));

}

void GP2Scene::AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, 
	const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue)
{

	assert((left < right ) && "Left is greater than right");
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

void GP2Scene::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, 
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

void GP2Scene::Draw(const VkCommandBuffer& cmdBuffer) const
{
	for (const auto& mesh : m_VecMeshes)
	{
		mesh.Draw(cmdBuffer);
	}
}

void GP2Scene::DestroyMeshes(const VkDevice& device)
{
	for (auto& mesh : m_VecMeshes)
	{
		mesh.DestroyMesh(device);
	}
}

void GP2Scene::MakeRect(GP2Mesh& mesh, float top, float left, float bottom, float right) const
{
	/*assert((left < right) && "Left is greater than right");
	assert((top < bottom) && "Top is greater than bottom");
	constexpr int nrOfVertices{ 4 };
	constexpr int nrOfIndices{ 6 };

	Vertex2D vertices[nrOfVertices]{ {glm::vec2{left, top},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{right, top},glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec2{right, bottom},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{left, bottom},glm::vec3{0.0f,0.0f,1.0f}} };

	int indices[nrOfIndices]{ 0,1,2,0,2,3 };

	for (int i = 0; i < nrOfVertices; i++) mesh.AddVertex(vertices[i].pos, vertices[i].color);
	for (int i = 0; i < nrOfIndices; i++) mesh.AddIndex(indices[i]);*/
}
