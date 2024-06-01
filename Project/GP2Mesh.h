#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "ContextStructs.h"
#include "GP2Buffer.h"
#include "Camera.h"
#include "GP2Texture.h"
#include "GP2Material.h"
#include "GP2CommandPool.h"
#include "GP2CommandBuffer.h"
#include <vector>
#include <algorithm>
#include "Vertex.h"
#include "OBJParser.h"
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtx/matrix_decompose.hpp>

class GP2CommandPool;

template <typename VertexType>
class GP2Mesh final
{
public:
	
	GP2Mesh() = default;
	~GP2Mesh() = default;

	void DestroyMesh(const VkDevice& device);
	void Draw(VkPipelineLayout pipelineLayout, const VkCommandBuffer& cmdBuffer) const;
	void UploadBuffers(const MeshContext& meshContext);

	void AddVertex(const VertexType& vertex);
	void AddIndex(uint32_t index);

	void SetModelMatrix(const MeshData& meshData);
	void SetTranslation(const glm::vec3& translation);

	GP2Material* GetMaterial() const;
	void SetMaterial(GP2Material* newMaterial);
private:

	void CopyBuffer(const MeshContext& meshContext, VkDeviceSize size, VkBuffer src, VkBuffer dst);

	GP2Material* m_pMaterial{nullptr};
	GP2Buffer m_VertexBuffer{};
	GP2Buffer m_IndexBuffer{};
	std::vector<VertexType> m_VecVertices{};
	std::vector<uint32_t> m_VecIndices{};

	glm::vec3 m_Translation{};

	MeshData m_VertexConstant{};
};

template <typename VertexType>
void GP2Mesh<VertexType>::UploadBuffers(const MeshContext& meshContext)
{
	if (m_VecIndices.size() % 3 != 0) throw std::runtime_error{ "Amount of indices is not a multiple of 3!" };

	VkDeviceSize size = sizeof(m_VecVertices[0]) * m_VecVertices.size();
	GP2Buffer stagingBuffer{};
	void* data;

	//VertexBuffer creation
	stagingBuffer.CreateBuffer(meshContext.device, meshContext.physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkMapMemory(meshContext.device, stagingBuffer.GetVkBufferMemory(), 0, size, 0, &data);
	memcpy(data, m_VecVertices.data(), (size_t)size);
	vkUnmapMemory(meshContext.device, stagingBuffer.GetVkBufferMemory());

	m_VertexBuffer.CreateBuffer(meshContext.device, meshContext.physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(meshContext, size, stagingBuffer.GetVkBuffer(), m_VertexBuffer.GetVkBuffer());

	stagingBuffer.DestroyBuffer(meshContext.device);



	//IndexBuffer creation
	size = sizeof(m_VecIndices[0]) * m_VecIndices.size();

	stagingBuffer.CreateBuffer(meshContext.device, meshContext.physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	vkMapMemory(meshContext.device, stagingBuffer.GetVkBufferMemory(), 0, size, 0, &data);
	memcpy(data, m_VecIndices.data(), (size_t)size);
	vkUnmapMemory(meshContext.device, stagingBuffer.GetVkBufferMemory());

	m_IndexBuffer.CreateBuffer(meshContext.device, meshContext.physicalDevice, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CopyBuffer(meshContext, size, stagingBuffer.GetVkBuffer(), m_IndexBuffer.GetVkBuffer());

	stagingBuffer.DestroyBuffer(meshContext.device);
}

template <typename VertexType>
void GP2Mesh<VertexType>::DestroyMesh(const VkDevice& device)
{
	m_pMaterial->m_Diffuse->DestroyTexture(device);
	m_pMaterial->m_Normal->DestroyTexture(device);
	m_pMaterial->m_Roughness->DestroyTexture(device);
	m_pMaterial->m_Specular->DestroyTexture(device);
	m_VertexBuffer.DestroyBuffer(device);
	m_IndexBuffer.DestroyBuffer(device);
}

template <typename VertexType>
void GP2Mesh<VertexType>::Draw(VkPipelineLayout pipelineLayout, const VkCommandBuffer& cmdBuffer) const
{

	m_VertexBuffer.BindAsVertexBuffer(cmdBuffer);
	m_IndexBuffer.BindAsIndexBuffer(cmdBuffer);

	vkCmdPushConstants(
		cmdBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT, // Stage flag should match the push constant range in the layout
		0, // Offset within the push constant block
		sizeof(MeshData), // Size of the push constants to update
		& m_VertexConstant // Pointer to the data
		);
	vkCmdPushConstants(
		cmdBuffer,
		pipelineLayout,
		VK_SHADER_STAGE_FRAGMENT_BIT, // Stage flag should match the push constant range in the layout
		sizeof(MeshData), // Offset within the push constant block
		sizeof(RenderProperties), // Size of the push constants to update
		&Camera::GetRenderProperties()// Pointer to the data
	);

	vkCmdDrawIndexed(cmdBuffer, static_cast<uint32_t>(m_VecIndices.size()), 1, 0, 0, 0);
}

template <typename VertexType>
void GP2Mesh<VertexType>::CopyBuffer(const MeshContext& meshContext, VkDeviceSize size, VkBuffer src, VkBuffer dst)
{
	GP2CommandBuffer cmdBuffer = meshContext.commandPool.CreateCommandBuffer(meshContext.device);

	cmdBuffer.BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(cmdBuffer.GetVkCommandBuffer(), src, dst, 1, &copyRegion);

	cmdBuffer.EndSingleTimeCommands(meshContext);
}

template <typename VertexType>
void GP2Mesh<VertexType>::AddVertex(const VertexType& vertex)
{
	m_VecVertices.push_back(vertex);
}

template <typename VertexType>
void GP2Mesh<VertexType>::AddIndex(uint32_t index)
{
	m_VecIndices.push_back(index);
}

template<typename VertexType>
void GP2Mesh<VertexType>::SetModelMatrix(const MeshData& meshData)
{
	/*glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(meshData.model, scale, rotation, translation, skew, perspective);*/

	MeshData newModel{ meshData };
	newModel.model[3] += glm::vec4(m_Translation,0.f);

	m_VertexConstant = newModel;
}

template<typename VertexType>
void GP2Mesh<VertexType>::SetTranslation(const glm::vec3& translation)
{
	m_Translation = translation;
	SetModelMatrix(MeshData{});
}

template<typename VertexType>
GP2Material* GP2Mesh<VertexType>::GetMaterial() const
{
	return m_pMaterial;
}

template<typename VertexType>
void GP2Mesh<VertexType>::SetMaterial(GP2Material* newMaterial)
{
	m_pMaterial = newMaterial;
}

inline static GP2Mesh<Vertex3D> CreateMesh(const std::string& objFile, const MeshContext& meshContext, bool flipAxisWinding = true)
{

	GP2Mesh<Vertex3D> mesh{};
	std::vector<Vertex3D> vertices{};
	std::vector<uint32_t> indices{};

	ParseOBJ(objFile, vertices, indices, flipAxisWinding);

	for (auto& vertex : vertices)
	{
		vertex.color = glm::vec3{ 1.f,1.f,1.f };
		mesh.AddVertex(vertex);
	}
	for (const auto& index : indices) mesh.AddIndex(index);

	mesh.UploadBuffers(meshContext);

	return mesh;
	
}

inline static GP2Mesh<Vertex3D> CreateBeam(float width, float height, float depth, const MeshContext& meshContext)
{
	GP2Mesh<Vertex3D> beam{};

	uint32_t currIndex{};

	std::vector<Vertex3D> officialVertices{};
	std::vector<uint32_t> officialIndices{};
	auto addFace = [&](std::array<Vertex3D, 4>& arrVertices) 
		{
			for (size_t vertex = 0; vertex < arrVertices.size(); vertex++)
			{
				switch (vertex)
				{
				case 0:
					arrVertices[vertex].texCoord = glm::vec2{};
					break;
				case 1:
					arrVertices[vertex].texCoord = glm::vec2{1,0};
					break;
				case 2:
					arrVertices[vertex].texCoord = glm::vec2{1,1};
					break;
				case 3:
					arrVertices[vertex].texCoord = glm::vec2{0,1};
					break;
				}
				
				officialVertices.push_back(arrVertices[vertex]);
			}

			officialIndices.push_back(currIndex);
			officialIndices.push_back(currIndex + 1);
			officialIndices.push_back(currIndex + 2);

			officialIndices.push_back(currIndex);
			officialIndices.push_back(currIndex + 2);
			officialIndices.push_back(currIndex + 3);

			currIndex += 4;
		};

	std::array<Vertex3D, 4> arrVertices{};

	arrVertices[0].pos = glm::vec3{ - width / 2,+ height / 2, + depth / 2 };
	arrVertices[1].pos = glm::vec3{ + width / 2,+ height / 2, + depth / 2 };
	arrVertices[2].pos = glm::vec3{ + width / 2,+ height / 2, - depth / 2 };
	arrVertices[3].pos = glm::vec3{ - width / 2,+ height / 2, - depth / 2 };
	for (size_t vertex = 0; vertex < arrVertices.size(); vertex++) arrVertices[vertex].normal = glm::vec3{ 0.f,1.f,0.f };

	addFace(arrVertices);

	arrVertices[0].pos = glm::vec3{ - width / 2,+ height / 2, - depth / 2 };
	arrVertices[1].pos = glm::vec3{ + width / 2,+ height / 2, - depth / 2 };
	arrVertices[2].pos = glm::vec3{ + width / 2,- height / 2, - depth / 2 };
	arrVertices[3].pos = glm::vec3{ - width / 2,- height / 2, - depth / 2 };
	for (size_t vertex = 0; vertex < arrVertices.size(); vertex++) arrVertices[vertex].normal = glm::vec3{ 0.f,0.f,-1.f };

	addFace(arrVertices);

	arrVertices[0].pos = glm::vec3{ - width / 2,- height / 2, - depth / 2 };
	arrVertices[1].pos = glm::vec3{ + width / 2,- height / 2, - depth / 2 };
	arrVertices[2].pos = glm::vec3{ + width / 2,- height / 2, + depth / 2 };
	arrVertices[3].pos = glm::vec3{ - width / 2,- height / 2, + depth / 2 };
	for (size_t vertex = 0; vertex < arrVertices.size(); vertex++) arrVertices[vertex].normal = glm::vec3{ 0.f,-1.f,0.f };

	addFace(arrVertices);

	arrVertices[0].pos = glm::vec3{ - width / 2,- height / 2, + depth / 2 };
	arrVertices[1].pos = glm::vec3{ + width / 2,- height / 2, + depth / 2 };
	arrVertices[2].pos = glm::vec3{ + width / 2,+ height / 2, + depth / 2 };
	arrVertices[3].pos = glm::vec3{ - width / 2,+ height / 2, + depth / 2 };
	for (size_t vertex = 0; vertex < arrVertices.size(); vertex++) arrVertices[vertex].normal = glm::vec3{ 0.f,0.f,1.f };

	addFace(arrVertices);

	arrVertices[0].pos = glm::vec3{ - width / 2,+ height / 2, + depth / 2 };
	arrVertices[1].pos = glm::vec3{ - width / 2,+ height / 2, - depth / 2 };
	arrVertices[2].pos = glm::vec3{ - width / 2,- height / 2, - depth / 2 };
	arrVertices[3].pos = glm::vec3{ - width / 2,- height / 2, + depth / 2 };
	for (size_t vertex = 0; vertex < arrVertices.size(); vertex++) arrVertices[vertex].normal = glm::vec3{ -1.f,0.f,0.f };

	addFace(arrVertices);

	arrVertices[0].pos = glm::vec3{ + width / 2,+ height / 2, - depth / 2 };
	arrVertices[1].pos = glm::vec3{ + width / 2,+ height / 2, + depth / 2 };
	arrVertices[2].pos = glm::vec3{ + width / 2,- height / 2, + depth / 2 };
	arrVertices[3].pos = glm::vec3{ + width / 2,- height / 2, - depth / 2 };
	for (size_t vertex = 0; vertex < arrVertices.size(); vertex++) arrVertices[vertex].normal = glm::vec3{ 1.f,0.f,0.f };

	addFace(arrVertices);

	CalculateTangents(officialVertices, officialIndices);

	for (size_t i = 0; i < officialVertices.size(); i++) beam.AddVertex(officialVertices[i]);

	for (size_t i = 0; i < officialIndices.size(); i++)	beam.AddIndex(officialIndices[i]);

	beam.UploadBuffers(meshContext);

	return beam;
}
inline static GP2Mesh<Vertex3D> CreateCube(float size, const MeshContext& meshContext)
{
	return CreateBeam(size, size, size, meshContext);
}

inline static GP2Mesh<Vertex3D> CreateSphere(float radius, int nrOfXDivisions, int nrOfYDivisions, const MeshContext& meshContext)
{
	if (nrOfXDivisions < 3 || nrOfYDivisions < 3) throw std::runtime_error{ "Divisions for the sphere are less than 3." };

	GP2Mesh<Vertex3D> sphere{};

	uint32_t currIndex{};

	constexpr double pi{ glm::pi<double>() };
	float stepHeight{ radius / (nrOfYDivisions / 2) };
	double stepAngle{ pi / nrOfYDivisions };
	Vertex3D currEdgeVertex{};

	
	std::vector<Vertex3D> officialVertices{  };
	std::vector<uint32_t> officialIndices{  };

	std::vector<Vertex3D> firstVertices{  };

	firstVertices.resize(nrOfYDivisions +1);

	for (size_t yIndex = 0; yIndex < firstVertices.size(); yIndex++)
	{
		float cosValue = static_cast<float>(glm::cos(stepAngle * (yIndex) - (pi / 2)));
		float sinValue = static_cast<float>(glm::sin(stepAngle * (yIndex) - (pi / 2)));

		currEdgeVertex.pos.x = radius * cosValue;
		currEdgeVertex.pos.y = radius * sinValue;
		currEdgeVertex.pos.z = 0.f;

		firstVertices[yIndex] = currEdgeVertex;
	
	}


	for (size_t yIndex = 0; yIndex < firstVertices.size(); yIndex++)
	{
		std::vector<Vertex3D> xDivisionVertices{  };
		xDivisionVertices.resize(nrOfXDivisions+1);
		for (int xIndex = 0; xIndex < xDivisionVertices.size(); xIndex++)
		{

			float xRadius = std::abs(firstVertices[yIndex].pos.x);
			float stepXAngle{ static_cast<float>(pi * 2 / nrOfXDivisions) };

			auto cosValue = glm::cos(stepXAngle * (xIndex));
			auto sinValue = glm::sin(stepXAngle * (xIndex));

			xDivisionVertices[xIndex].pos.x = (xRadius * cosValue);
			xDivisionVertices[xIndex].pos.y = firstVertices[yIndex].pos.y;
			xDivisionVertices[xIndex].pos.z = (xRadius * sinValue);

			xDivisionVertices[xIndex].texCoord = glm::vec2{ (1.f / nrOfXDivisions) * xIndex, (1.f / nrOfYDivisions) * yIndex };
			xDivisionVertices[xIndex].normal = glm::vec3{ glm::normalize(xDivisionVertices[xIndex].pos )};
			officialVertices.push_back(xDivisionVertices[xIndex]);

			if(yIndex < firstVertices.size() - 1 && xIndex < xDivisionVertices.size() - 1)
			{

				officialIndices.push_back(currIndex);
				officialIndices.push_back(currIndex + nrOfXDivisions + 1);
				officialIndices.push_back(currIndex + 1);

				officialIndices.push_back(currIndex + nrOfXDivisions + 1);
				officialIndices.push_back(currIndex + nrOfXDivisions + 1 + 1);
				officialIndices.push_back(currIndex + 1);
			}

			++currIndex;
		}

	}

	CalculateTangents(officialVertices, officialIndices);


	for (size_t i = 0; i < officialVertices.size(); i++) sphere.AddVertex(officialVertices[i]);
	
	for (size_t i = 0; i < officialIndices.size(); i++)	sphere.AddIndex(officialIndices[i]);

	sphere.UploadBuffers(meshContext);

	return sphere;

}

inline static GP2Mesh<Vertex2D> CreateRectangle(float top, float left, float bottom, float right, const MeshContext& meshContext)
{

	assert((left < right) && "Left is greater than right");
	assert((top < bottom) && "Top is greater than bottom");
	GP2Mesh<Vertex2D> rect{};
	constexpr int nrOfVertices{ 4 };
	constexpr int nrOfIndices{ 6 };

	Vertex2D vertices[nrOfVertices]{ {{left, top}, glm::vec3{1.0f,0.0f,0.0f}, {0.f,0.f}},
						{{right, top}, glm::vec3{0.0f,1.0f,0.0f}, {1.f,0.f}},
						{{right, bottom}, glm::vec3{1.0f,0.0f,0.0f}, {1.f,1.f}},
						{{left, bottom}, glm::vec3{0.0f,0.0f,1.0f}, {0.f,1.f}} };

	int indices[nrOfIndices]{ 0,1,2,0,2,3 };

	for (int i = 0; i < nrOfVertices; i++) rect.AddVertex(vertices[i]);
	for (int i = 0; i < nrOfIndices; i++) rect.AddIndex(indices[i]);
	
	rect.UploadBuffers(meshContext);

	return rect;
}

inline static GP2Mesh<Vertex2D> CreateRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const MeshContext& meshContext)
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

	float width = right - left;
	float height = bottom - top;

	uint32_t currMaxIndex{  };
	GP2Mesh<Vertex2D> rect{};

	//MiddleRect
	Vertex2D verticesMiddleRect[4]{};
	verticesMiddleRect[0] = { glm::vec2{left + radiusX, top + radiusY},glm::vec3{0.0f,0.0f,1.0f}, glm::vec2{radiusX / width, radiusY / height} };
	verticesMiddleRect[1] = { glm::vec2{right - radiusX, top + radiusY},glm::vec3{1.0f,0.0f,0.0f}, glm::vec2{(width - radiusX)/width, radiusY / height} };
	verticesMiddleRect[2] = { glm::vec2{ right - radiusX, bottom - radiusY }, glm::vec3{1.0f,0.0f,0.0f}, glm::vec2{(width - radiusX) / width, (height - radiusY) / height} };
	verticesMiddleRect[3] = { glm::vec2{ left + radiusX, bottom - radiusY },glm::vec3{0.0f,0.0f,1.0f}, glm::vec2{radiusX / width,(height - radiusY) / height} };

	rect.AddVertex(verticesMiddleRect[0]);
	rect.AddVertex(verticesMiddleRect[1]);
	rect.AddVertex(verticesMiddleRect[2]);
	rect.AddVertex(verticesMiddleRect[3]);
	currMaxIndex += 3;

	rect.AddIndex(0);
	rect.AddIndex(1);
	rect.AddIndex(2);

	rect.AddIndex(0);
	rect.AddIndex(2);
	rect.AddIndex(3);

	//TopRect
	Vertex2D verticesTopRect[2]{};
	verticesTopRect[0] = { glm::vec2{left + radiusX, top},glm::vec3{0.0f,0.0f,1.0f}, glm::vec2{radiusX / width, 0.f} };
	verticesTopRect[1] = { glm::vec2{right - radiusX, top},glm::vec3{1.0f,0.0f,0.0f}, glm::vec2{(width - radiusX) / width, 0.f} };

	rect.AddVertex(verticesTopRect[0]);
	rect.AddVertex(verticesTopRect[1]);
	currMaxIndex += 2;

	rect.AddIndex(4);
	rect.AddIndex(5);
	rect.AddIndex(1);

	rect.AddIndex(4);
	rect.AddIndex(1);
	rect.AddIndex(0);

	//LeftRect
	Vertex2D verticesLeftRect[2]{};
	verticesLeftRect[0] = { glm::vec2{ left, top + radiusY },glm::vec3{0.0f,1.0f,0.0f}, glm::vec2{0.f,radiusY / height } };
	verticesLeftRect[1] = { glm::vec2{ left, bottom - radiusY },glm::vec3{0.0f,1.0f,0.0f}, glm::vec2{0.f, (height - radiusY) / height } };

	rect.AddVertex(verticesLeftRect[0]);
	rect.AddVertex(verticesLeftRect[1]);
	currMaxIndex += 2;

	rect.AddIndex(6);
	rect.AddIndex(0);
	rect.AddIndex(3);

	rect.AddIndex(6);
	rect.AddIndex(3);
	rect.AddIndex(7);

	//RightRect
	Vertex2D verticesRightRect[2]{};
	verticesRightRect[0] = { glm::vec2{ right, top + radiusY },glm::vec3{0.0f,1.0f,0.0f}, glm::vec2{1.f, radiusY / height } };
	verticesRightRect[1] = { glm::vec2{ right, bottom - radiusY },glm::vec3{0.0f,1.0f,0.0f }, glm::vec2{1.f,(height - radiusY) / height } };

	rect.AddVertex(verticesRightRect[0]);
	rect.AddVertex(verticesRightRect[1]);
	currMaxIndex += 2;

	rect.AddIndex(1);
	rect.AddIndex(8);
	rect.AddIndex(9);

	rect.AddIndex(1);
	rect.AddIndex(9);
	rect.AddIndex(2);

	//BottomRect
	Vertex2D verticesBottomRect[2]{};
	verticesBottomRect[0] = { glm::vec2{ left + radiusX, bottom },glm::vec3{0.0f,0.0f,1.0f}, glm::vec2{radiusX/width, 1.f} };
	verticesBottomRect[1] = { glm::vec2{ right - radiusX, bottom },glm::vec3{1.0f,0.0f,0.0f }, glm::vec2{(width - radiusX) / width, 1.f} };

	rect.AddVertex(verticesBottomRect[0]);
	rect.AddVertex(verticesBottomRect[1]);
	currMaxIndex += 2;

	rect.AddIndex(3);
	rect.AddIndex(2);
	rect.AddIndex(11);

	rect.AddIndex(3);
	rect.AddIndex(11);
	rect.AddIndex(10);



	//CORNERS
	Vertex2D currEdgeVertex;
	constexpr float pi = 3.14159265359f;
	float radians = pi / 2 / numberOfSegmentsPerCorner;
	float mappedRadiusX = (radiusX / width);
	float mappedRadiusY = (radiusX / height);

	//TopLeftCornerY

	rect.AddIndex(0);
	rect.AddIndex(6);

	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	{
		auto cosValue = glm::cos(radians * i + pi);
		auto sinValue = glm::sin(radians * i + pi);

		currEdgeVertex.texCoord.x = verticesMiddleRect[0].texCoord.x + cosValue * mappedRadiusX;
		currEdgeVertex.texCoord.y = verticesMiddleRect[0].texCoord.y + sinValue * mappedRadiusY;

		currEdgeVertex.pos.x = verticesMiddleRect[0].pos.x + radiusX * cosValue;
		currEdgeVertex.pos.y = verticesMiddleRect[0].pos.y + radiusY * sinValue;

		rect.AddVertex(currEdgeVertex);
		++currMaxIndex;

		//triangle
		rect.AddIndex(currMaxIndex);
		if (i > 1)
		{
			rect.AddIndex(0);
			rect.AddIndex(currMaxIndex - 1);
		}
	}

	rect.AddIndex(0);
	rect.AddIndex(currMaxIndex);
	rect.AddIndex(4);


	//TopRightCorner
	rect.AddIndex(1);
	rect.AddIndex(5);

	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	{

		auto cosValue = glm::cos(radians * i - pi / 2);
		auto sinValue = glm::sin(radians * i - pi / 2);

		currEdgeVertex.texCoord.x = verticesMiddleRect[1].texCoord.x + cosValue * mappedRadiusX;
		currEdgeVertex.texCoord.y = verticesMiddleRect[1].texCoord.y + sinValue * mappedRadiusY;

		currEdgeVertex.pos.x = verticesMiddleRect[1].pos.x + radiusX * cosValue;
		currEdgeVertex.pos.y = verticesMiddleRect[1].pos.y + radiusY * sinValue;

		rect.AddVertex(currEdgeVertex);
		++currMaxIndex;

		//triangle
		rect.AddIndex(currMaxIndex);
		if (i > 1)
		{
			rect.AddIndex(1);
			rect.AddIndex(currMaxIndex - 1);
		}
	}

	rect.AddIndex(1);
	rect.AddIndex(currMaxIndex);
	rect.AddIndex(8);


	//BottomLeftCorner
	rect.AddIndex(3);
	rect.AddIndex(10);

	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	{
		auto cosValue = glm::cos(radians * i + pi / 2);
		auto sinValue = glm::sin(radians * i + pi / 2);

		currEdgeVertex.texCoord.x = verticesMiddleRect[3].texCoord.x + cosValue * mappedRadiusX;
		currEdgeVertex.texCoord.y = verticesMiddleRect[3].texCoord.y + sinValue * mappedRadiusY;

		currEdgeVertex.pos.x = verticesMiddleRect[3].pos.x + radiusX * cosValue;
		currEdgeVertex.pos.y = verticesMiddleRect[3].pos.y + radiusY * sinValue;

		rect.AddVertex(currEdgeVertex);
		++currMaxIndex;

		//triangle
		rect.AddIndex(currMaxIndex);
		if (i > 1)
		{
			rect.AddIndex(3);
			rect.AddIndex(currMaxIndex - 1);
		}
	}

	rect.AddIndex(3);
	rect.AddIndex(currMaxIndex);
	rect.AddIndex(7);


	//BottomRightCorner
	rect.AddIndex(2);
	rect.AddIndex(9);

	currEdgeVertex.color = glm::vec3{ 1.0f, 1.0f, 1.0f };

	for (int i = 1; i < numberOfSegmentsPerCorner; i++)
	{
		auto cosValue = glm::cos(radians * i);
		auto sinValue = glm::sin(radians * i);

		currEdgeVertex.texCoord.x = verticesMiddleRect[2].texCoord.x + cosValue * mappedRadiusX;
		currEdgeVertex.texCoord.y = verticesMiddleRect[2].texCoord.y + sinValue * mappedRadiusY;

		currEdgeVertex.pos.x = verticesMiddleRect[2].pos.x + radiusX * cosValue;
		currEdgeVertex.pos.y = verticesMiddleRect[2].pos.y + radiusY * sinValue;

		rect.AddVertex(currEdgeVertex);
		++currMaxIndex;

		//triangle
		rect.AddIndex(currMaxIndex);
		if (i > 1)
		{
			rect.AddIndex(2);
			rect.AddIndex(currMaxIndex - 1);
		}
	}

	rect.AddIndex(2);
	rect.AddIndex(currMaxIndex);
	rect.AddIndex(11);


	rect.UploadBuffers(meshContext);
	return rect;
}

inline static GP2Mesh<Vertex2D> CreateOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const MeshContext& meshContext)
{

	assert((radiusX > 0 && radiusY > 0));
	constexpr float pi = 3.14159265359f;

	float radians = pi * 2 / numberOfSegments;

	GP2Mesh<Vertex2D> oval{};

	Vertex2D center{ glm::vec2{centerX, centerY}, glm::vec3{0.0f,0.0f,1.0f},glm::vec2{0.5f,0.5f} };
	Vertex2D currEdgeVertex { {}, glm::vec3{0.0f,1.0f,0.0f} };



	oval.AddVertex(center);
	for (int i = 1; i <= numberOfSegments; i++)
	{
		auto cosValue = glm::cos(radians * i);
		auto sinValue = glm::sin(-radians * i);

		currEdgeVertex.texCoord.x = (cosValue+1)/2;
		currEdgeVertex.texCoord.y = (sinValue+1)/2;
		currEdgeVertex.pos.x = centerX + radiusX * cosValue;
		currEdgeVertex.pos.y = centerY + radiusY * sinValue;

		oval.AddVertex(currEdgeVertex);

		oval.AddIndex(i);
		oval.AddIndex(0);
		if (i == numberOfSegments) oval.AddIndex(1);
		else oval.AddIndex(i+1);
	}

	oval.UploadBuffers(meshContext);
	return oval;
}