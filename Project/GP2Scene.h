#pragma once
#include "vulkanbase/VulkanUtil.h"
#include <vector>
class GP2Mesh;
class GP2CommandPool;
class GP2Scene
{
public:
	GP2Scene() = default;
	~GP2Scene() = default;

	GP2Scene(const GP2Scene& other) = delete;
	GP2Scene(GP2Scene&& other) noexcept = delete;
	GP2Scene& operator=(const GP2Scene& other) = delete;
	GP2Scene& operator=(GP2Scene&& other) noexcept = delete;

	void AddMesh(const GP2Mesh& gp2Mesh);
	void AddRectangle(float top, float left, float bottom, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void Draw(const VkCommandBuffer& cmdBuffer) const;
	void DestroyMeshes(const VkDevice& device);
private:
	void MakeRect(GP2Mesh& mesh, float top, float left, float bottom, float right) const;
	std::vector<GP2Mesh> m_VecMeshes;
};
