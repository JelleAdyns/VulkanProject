#include "GP2Scene.h"
#include "GP2Mesh.h"
#include "Vertex.h"
#include <assert.h>

void GP2Scene::AddMesh(const GP2Mesh& gp2Mesh)
{
	//m_VecMeshes.push_back(gp2Mesh);
}

void GP2Scene::AddRectangle(float top, float left, float bottom, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
	assert((left < right) && "Left is greater than right");
	assert((top < bottom) && "Top is greater than bottom");
	Vertex vertices[4]{ {glm::vec2{left, top},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{right, top},glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec2{right, bottom},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{left, bottom},glm::vec3{0.0f,0.0f,1.0f}} };

	GP2Mesh rect{};
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);
	rect.Initialize(physicalDevice, device);
	m_VecMeshes.push_back(std::move(rect));
}

void GP2Scene::AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{

	assert((left < right ) && "Left is greater than right");
	assert((top < bottom) && "Top is greater than bottom");
	assert((radiusX > 0 && radiusY > 0) && "Radius is less or equal than 0");

	constexpr float pi = 3.14159265359f;

	float radians = pi / 2 / numberOfSegmentsPerCorner;

	//Upper Rect
	Vertex verticesTopRect[4]{ {glm::vec2{left + radiusX, top},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{right - radiusX, top},glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec2{right - radiusX, top + radiusY},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{left + radiusX, top + radiusY},glm::vec3{0.0f,0.0f,1.0f}} };

	GP2Mesh rect{};
	rect.AddVertex(verticesTopRect[0].pos, verticesTopRect[0].color);
	rect.AddVertex(verticesTopRect[1].pos, verticesTopRect[1].color);
	rect.AddVertex(verticesTopRect[2].pos, verticesTopRect[2].color);
	rect.AddVertex(verticesTopRect[0].pos, verticesTopRect[0].color);
	rect.AddVertex(verticesTopRect[2].pos, verticesTopRect[2].color);
	rect.AddVertex(verticesTopRect[3].pos, verticesTopRect[3].color);

	Vertex startPoint;
	Vertex endPoint;

	startPoint.color = glm::vec3{ 1.0f, 0.f, 0.f };
	endPoint.color = glm::vec3{ 0.0f, 1.f, 0.f };

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = verticesTopRect[3].pos.x + radiusX * glm::cos(radians * i + pi);
		startPoint.pos.y = verticesTopRect[3].pos.y + radiusY * glm::sin(radians * i + pi);

		endPoint.pos.x = verticesTopRect[3].pos.x + radiusX * glm::cos(radians * (i+1) + pi);
		endPoint.pos.y = verticesTopRect[3].pos.y + radiusY * glm::sin(radians * (i+1) + pi);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(verticesTopRect[3].pos, verticesTopRect[3].color);
	}

	//Left Rect
	Vertex verticesLeftRect[4]{};
	verticesLeftRect[0] = { glm::vec2{ left, top + radiusY },glm::vec3{1.0f,0.0f,0.0f} };
	verticesLeftRect[1] = verticesTopRect[3];
	verticesLeftRect[2] = { glm::vec2{ left + radiusX, bottom - radiusY },glm::vec3{1.0f,0.0f,0.0f} };
	verticesLeftRect[3] = { glm::vec2{ left, bottom - radiusY },glm::vec3{0.0f,0.0f,1.0f }};

	rect.AddVertex(verticesLeftRect[0].pos, verticesLeftRect[0].color);
	rect.AddVertex(verticesLeftRect[1].pos, verticesLeftRect[1].color);
	rect.AddVertex(verticesLeftRect[2].pos, verticesLeftRect[2].color);
	rect.AddVertex(verticesLeftRect[0].pos, verticesLeftRect[0].color);
	rect.AddVertex(verticesLeftRect[2].pos, verticesLeftRect[2].color);
	rect.AddVertex(verticesLeftRect[3].pos, verticesLeftRect[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = verticesLeftRect[2].pos.x + radiusX * glm::cos(radians * i + pi / 2);
		startPoint.pos.y = verticesLeftRect[2].pos.y + radiusY * glm::sin(radians * i + pi / 2);

		endPoint.pos.x = verticesLeftRect[2].pos.x + radiusX * glm::cos(radians * (i + 1) + pi / 2);
		endPoint.pos.y = verticesLeftRect[2].pos.y + radiusY * glm::sin(radians * (i + 1) + pi / 2);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(verticesLeftRect[2].pos, verticesLeftRect[2].color);
	}
	

	//Right Rect
	Vertex verticesRightRect[4]{};
	verticesRightRect[0] = verticesTopRect[2];
	verticesRightRect[1] = { glm::vec2{ right, top + radiusY }, glm::vec3{0.0f,1.0f,0.0f} };
	verticesRightRect[2] = { glm::vec2{ right, bottom - radiusY }, glm::vec3{1.0f,0.0f,0.0f} };
	verticesRightRect[3] = { glm::vec2{ right - radiusX, bottom - radiusY }, glm::vec3{0.0f,0.0f,1.0f} };

	rect.AddVertex(verticesRightRect[0].pos, verticesRightRect[0].color);
	rect.AddVertex(verticesRightRect[1].pos, verticesRightRect[1].color);
	rect.AddVertex(verticesRightRect[2].pos, verticesRightRect[2].color);
	rect.AddVertex(verticesRightRect[0].pos, verticesRightRect[0].color);
	rect.AddVertex(verticesRightRect[2].pos, verticesRightRect[2].color);
	rect.AddVertex(verticesRightRect[3].pos, verticesRightRect[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = verticesRightRect[0].pos.x + radiusX * glm::cos(radians * i - pi / 2);
		startPoint.pos.y = verticesRightRect[0].pos.y + radiusY * glm::sin(radians * i - pi / 2);

		endPoint.pos.x = verticesRightRect[0].pos.x + radiusX * glm::cos(radians * (i + 1) - pi / 2);
		endPoint.pos.y = verticesRightRect[0].pos.y + radiusY * glm::sin(radians * (i + 1) - pi / 2);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(verticesRightRect[0].pos, verticesRightRect[0].color);
	}
	
	//Bottom Rect
	Vertex verticesBottomRect[4]{};
	verticesBottomRect[0] = verticesLeftRect[2];
	verticesBottomRect[1] = verticesRightRect[3];
	verticesBottomRect[2] = { glm::vec2{ right - radiusX, bottom } ,glm::vec3{1.0f,0.0f,0.0f} };
	verticesBottomRect[3] = { glm::vec2{ left + radiusX, bottom }, glm::vec3{0.0f,0.0f,1.0f} };

	rect.AddVertex(verticesBottomRect[0].pos, verticesBottomRect[0].color);
	rect.AddVertex(verticesBottomRect[1].pos, verticesBottomRect[1].color);
	rect.AddVertex(verticesBottomRect[2].pos, verticesBottomRect[2].color);
	rect.AddVertex(verticesBottomRect[0].pos, verticesBottomRect[0].color);
	rect.AddVertex(verticesBottomRect[2].pos, verticesBottomRect[2].color);
	rect.AddVertex(verticesBottomRect[3].pos, verticesBottomRect[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = verticesBottomRect[1].pos.x + radiusX * glm::cos(radians * i);
		startPoint.pos.y = verticesBottomRect[1].pos.y + radiusY * glm::sin(radians * i);

		endPoint.pos.x = verticesBottomRect[1].pos.x + radiusX * glm::cos(radians * (i + 1));
		endPoint.pos.y = verticesBottomRect[1].pos.y + radiusY * glm::sin(radians * (i + 1));

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(verticesBottomRect[1].pos, verticesBottomRect[1].color);
	}

	//Middle Rect
	Vertex verticesMiddleRect[4]{};
	verticesMiddleRect[0] = verticesTopRect[3];
	verticesMiddleRect[1] = verticesRightRect[0];
	verticesMiddleRect[2] = verticesBottomRect[1];
	verticesMiddleRect[3] = verticesLeftRect[2];

	rect.AddVertex(verticesMiddleRect[0].pos, verticesMiddleRect[0].color);
	rect.AddVertex(verticesMiddleRect[1].pos, verticesMiddleRect[1].color);
	rect.AddVertex(verticesMiddleRect[2].pos, verticesMiddleRect[2].color);
	rect.AddVertex(verticesMiddleRect[0].pos, verticesMiddleRect[0].color);
	rect.AddVertex(verticesMiddleRect[2].pos, verticesMiddleRect[2].color);
	rect.AddVertex(verticesMiddleRect[3].pos, verticesMiddleRect[3].color);

	rect.Initialize(physicalDevice, device);
	m_VecMeshes.push_back(std::move(rect));
}

void GP2Scene::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{

	assert((radiusX > 0 && radiusY > 0));
	constexpr float pi = 3.14159265359f;

	float radians = pi * 2 / numberOfSegments;

	Vertex center  {glm::vec2{centerX, centerY}, glm::vec3{0.0f,0.0f,1.0f}};
	Vertex startPoint { glm::vec2{}, glm::vec3{0.0f,1.0f,0.0f} };
	Vertex endPoint { glm::vec2{}, glm::vec3{1.0f,0.0f,0.0f} };
	
	GP2Mesh oval;
	for (int i = 0; i < numberOfSegments; i++)
	{
		startPoint.pos.x = centerX + radiusX * glm::cos(radians * i);
		startPoint.pos.y = centerY + radiusY * glm::sin(radians * i);
		endPoint.pos.x = centerX + radiusX * glm::cos(radians * (i+1));
		endPoint.pos.y = centerY + radiusY * glm::sin(radians * (i+1));

		oval.AddVertex(startPoint.pos, startPoint.color);
		oval.AddVertex(endPoint.pos, endPoint.color);
		oval.AddVertex(center.pos, center.color);

	}

	oval.Initialize(physicalDevice, device);
	m_VecMeshes.push_back(std::move(oval));
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
