#pragma once
#include "GP2GraphicsPipeline.h"
#include "GP2Shader3D.h"
#include "GP2Mesh.h"
#include <vector>

class GP2GraphicsPipeline3D final : public GP2GraphicsPipeline
{
public:
	GP2GraphicsPipeline3D() = default;
	virtual ~GP2GraphicsPipeline3D() = default;

	GP2GraphicsPipeline3D(const GP2GraphicsPipeline3D& other) = delete;
	GP2GraphicsPipeline3D(GP2GraphicsPipeline3D&& other) noexcept = delete;
	GP2GraphicsPipeline3D& operator=(const GP2GraphicsPipeline3D& other) = delete;
	GP2GraphicsPipeline3D& operator=(GP2GraphicsPipeline3D&& other) noexcept = delete;

	void Initialize(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat, GP2Shader3D& shader);

	void AddMesh(const std::string& objFile, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddRectangle(float top, float left, float bottom, float right, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const VkPhysicalDevice& physicalDevice, const VkDevice& device, const GP2CommandPool& commandPool, VkQueue graphicsQueue);
	void Draw(const VkCommandBuffer& cmdBuffer) const;
	void DestroyMeshes(const VkDevice& device);
private:

	std::vector<GP2Mesh<Vertex3D>> m_VecMeshes;

	void MakeRect(GP2Mesh<Vertex3D>& mesh, float top, float left, float bottom, float right) const;
	void CreateRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);
	void CreateGraphicsPipeline(const VkDevice& device, GP2Shader3D& shader);

};
