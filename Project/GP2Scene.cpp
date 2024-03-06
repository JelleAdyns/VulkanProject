#include "GP2Scene.h"
#include "GP2Mesh.h"
#include "Vertex.h"

void GP2Scene::AddMesh(const GP2Mesh& gp2Mesh)
{
	//m_VecMeshes.push_back(gp2Mesh);
}

void GP2Scene::AddRectangle(float top, float left, float bottom, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
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
	constexpr float pi = 3.14159265359f;

	float radians = pi / 2 / numberOfSegmentsPerCorner;

	//Upper Rect
	Vertex vertices[4]{ {glm::vec2{left + radiusX, top},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{right - radiusX, top},glm::vec3{0.0f,1.0f,0.0f}},
						{glm::vec2{right - radiusX, top + radiusY},glm::vec3{1.0f,0.0f,0.0f}},
						{glm::vec2{left + radiusX, top + radiusY},glm::vec3{0.0f,0.0f,1.0f}} };

	GP2Mesh rect{};
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	Vertex startPoint;
	Vertex endPoint;

	startPoint.color = glm::vec3{ 1.0f, 0.f, 0.f };
	endPoint.color = glm::vec3{ 0.0f, 1.f, 0.f };

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[3].pos.x + radiusX * glm::cos(radians * i + pi);
		startPoint.pos.y = vertices[3].pos.y + radiusY * glm::sin(radians * i + pi);

		endPoint.pos.x = vertices[3].pos.x + radiusX * glm::cos(radians * (i+1) + pi);
		endPoint.pos.y = vertices[3].pos.y + radiusY * glm::sin(radians * (i+1) + pi);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[3].pos, vertices[3].color);
	}

	//Left Rect
	vertices[0].pos = glm::vec2{left, top + radiusY};
	vertices[1].pos = glm::vec2{left + radiusX, top + radiusY};
	vertices[2].pos = glm::vec2{left + radiusX, bottom - radiusY};
	vertices[3].pos = glm::vec2{left, bottom - radiusY};

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[2].pos.x + radiusX * glm::cos(radians * i + pi/2);
		startPoint.pos.y = vertices[2].pos.y + radiusY * glm::sin(radians * i + pi/2);

		endPoint.pos.x = vertices[2].pos.x + radiusX * glm::cos(radians * (i + 1) + pi/2);
		endPoint.pos.y = vertices[2].pos.y + radiusY * glm::sin(radians * (i + 1) + pi/2);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[2].pos, vertices[2].color);
	}
	//Middle Rect
	vertices[0].pos = glm::vec2{left + radiusX, top + radiusY};
	vertices[1].pos = glm::vec2{right - radiusX, top + radiusY};
	vertices[2].pos = glm::vec2{right - radiusX, bottom - radiusY};
	vertices[3].pos = glm::vec2{left + radiusX, bottom - radiusY};

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	//Right Rect
	vertices[0].pos = glm::vec2{right - radiusX, top + radiusY};
	vertices[1].pos = glm::vec2{right, top + radiusY};
	vertices[2].pos = glm::vec2{right, bottom - radiusY };
	vertices[3].pos = glm::vec2{right - radiusX, bottom - radiusY};

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[0].pos.x + radiusX * glm::cos(radians * i - pi / 2);
		startPoint.pos.y = vertices[0].pos.y + radiusY * glm::sin(radians * i - pi / 2);

		endPoint.pos.x = vertices[0].pos.x + radiusX * glm::cos(radians * (i + 1) - pi / 2);
		endPoint.pos.y = vertices[0].pos.y + radiusY * glm::sin(radians * (i + 1) - pi / 2);

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[0].pos, vertices[0].color);
	}
	
	//Bottom Rect
	vertices[0].pos = glm::vec2{left + radiusX, bottom - radiusY};
	vertices[1].pos = glm::vec2{right - radiusX, bottom - radiusY};
	vertices[2].pos = glm::vec2{right - radiusX, bottom };
	vertices[3].pos = glm::vec2{left + radiusX, bottom };

	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[1].pos, vertices[1].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[0].pos, vertices[0].color);
	rect.AddVertex(vertices[2].pos, vertices[2].color);
	rect.AddVertex(vertices[3].pos, vertices[3].color);

	for (int i = 0; i < numberOfSegmentsPerCorner; i++)
	{
		startPoint.pos.x = vertices[1].pos.x + radiusX * glm::cos(radians * i );
		startPoint.pos.y = vertices[1].pos.y + radiusY * glm::sin(radians * i );

		endPoint.pos.x = vertices[1].pos.x + radiusX * glm::cos(radians * (i + 1) );
		endPoint.pos.y = vertices[1].pos.y + radiusY * glm::sin(radians * (i + 1) );

		rect.AddVertex(startPoint.pos, startPoint.color);
		rect.AddVertex(endPoint.pos, endPoint.color);
		rect.AddVertex(vertices[1].pos, vertices[1].color);
	}


	rect.Initialize(physicalDevice, device);
	m_VecMeshes.push_back(std::move(rect));
}

void GP2Scene::AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device)
{
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
