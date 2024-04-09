#pragma once
#include "GP2GraphicsPipeline.h"
#include "GP2Shader2D.h"
#include "GP2Mesh.h"
#include <vector>

class GP2GraphicsPipeline2D final : public GP2GraphicsPipeline
{
public:
	GP2GraphicsPipeline2D() = default;
	virtual ~GP2GraphicsPipeline2D() = default;

	GP2GraphicsPipeline2D(const GP2GraphicsPipeline2D& other) = delete;
	GP2GraphicsPipeline2D(GP2GraphicsPipeline2D&& other) noexcept = delete;
	GP2GraphicsPipeline2D& operator=(const GP2GraphicsPipeline2D& other) = delete;
	GP2GraphicsPipeline2D& operator=(GP2GraphicsPipeline2D&& other) noexcept = delete;

	void Initialize(const VkDevice& device, const VkFormat& swapChainImageFormat, GP2Shader2D& shader);
	void AddMesh(const GP2Mesh<Vertex2D>& gp2Mesh);
	void AddRectangle(float top, float left, float bottom, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void Draw(const VkCommandBuffer& cmdBuffer) const;
	void DestroyMeshes(const VkDevice& device);
private:

	std::vector<GP2Mesh<Vertex2D>> m_VecMeshes;

	void MakeRect(GP2Mesh<Vertex2D>& mesh, float top, float left, float bottom, float right) const;

	void CreateRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat);
	void CreateGraphicsPipeline(const VkDevice& device, GP2Shader2D& shader);

};
