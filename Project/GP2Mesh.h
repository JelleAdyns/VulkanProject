#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "glm/glm.hpp"
#include <vector>
#include "Vertex.h"
#include "GP2Buffer.h"

class GP2CommandPool;
class GP2Mesh
{
public:
	GP2Mesh() : m_VertexBuffer{}, m_StagingBuffer{}, m_VecVertices {}
	{}
	~GP2Mesh() = default;

	GP2Mesh(const GP2Mesh& other) = delete;
	GP2Mesh(GP2Mesh&& other) noexcept = default;
	GP2Mesh& operator=(const GP2Mesh& other) = delete;
	GP2Mesh& operator=(GP2Mesh&& other) noexcept = default;

	void Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);

	void DestroyMesh(const VkDevice& device);

	void Draw(const VkCommandBuffer& cmdBuffer) const;

	void AddVertex(glm::vec2 pos, glm::vec3 color);

private:

	void CopyBuffer(const VkDevice& device, const GP2CommandPool& commandPool, VkDeviceSize size, VkQueue graphicsQueue);
	GP2Buffer m_VertexBuffer;
	GP2Buffer m_StagingBuffer;
	std::vector<Vertex> m_VecVertices;
};
