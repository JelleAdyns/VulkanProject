#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "glm/glm.hpp"
#include <vector>
#include "Vertex.h"

class GP2Mesh
{
public:
	GP2Mesh() : m_VkBuffer{}, m_VkBufferMemory{}, m_VecVertices{}
	{}
	~GP2Mesh() = default;

	GP2Mesh(const GP2Mesh& other) = delete;
	GP2Mesh(GP2Mesh&& other) noexcept = default;
	GP2Mesh& operator=(const GP2Mesh& other) = delete;
	GP2Mesh& operator=(GP2Mesh&& other) noexcept = default;

	void Initialize(const VkPhysicalDevice& physicalDevice, const VkDevice& device);

	void DestroyMesh(const VkDevice& device);

	void Draw(const VkCommandBuffer& cmdBuffer) const;

	void AddVertex(glm::vec2 pos, glm::vec3 color);

private:
	uint32_t FindMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;

	VkBuffer m_VkBuffer;
	VkDeviceMemory m_VkBufferMemory;
	std::vector<Vertex> m_VecVertices;
};
