#pragma once
#include "GP2GraphicsPipeline.h"
#include "GP2Shader.h"
#include "GP2Mesh.h"
//#include "ContextStructs.h"
//#include <vector>
//
//class GP2GraphicsPipeline3D final : public GP2GraphicsPipeline
//{
//public:
//	GP2GraphicsPipeline3D(const std::string& vertexShaderFile, const std::string& fragmentShaderFile) :
//		GP2GraphicsPipeline{},
//		m_VecMeshes{},
//		m_Shader{ vertexShaderFile,fragmentShaderFile }
//	{}
//	virtual ~GP2GraphicsPipeline3D() = default;
//
//	GP2GraphicsPipeline3D(const GP2GraphicsPipeline3D& other) = delete;
//	GP2GraphicsPipeline3D(GP2GraphicsPipeline3D&& other) noexcept = delete;
//	GP2GraphicsPipeline3D& operator=(const GP2GraphicsPipeline3D& other) = delete;
//	GP2GraphicsPipeline3D& operator=(GP2GraphicsPipeline3D&& other) noexcept = delete;
//
//	void Initialize(const VulkanContext& vulkanContext, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);
//
//	void UpdateMeshMatrix(const glm::mat4& model, size_t meshIndex);
//	void UpdateUniformBuffer(uint32_t currentImage, glm::mat4 view, glm::mat4 projection);
//	void Record(const GP2CommandBuffer& cmdBuffer, VkExtent2D vkExtent) const;
//
//	void AddMesh(const std::string& objFile, const MeshContext& meshContext);
//	void AddRectangle(float top, float left, float bottom, float right, const MeshContext& meshContext);
//	void AddRoundedRectangle(float top, float left, float bottom, float right, float radiusX, float radiusY, int numberOfSegmentsPerCorner, const MeshContext& meshContext);
//	void AddOval(float centerX, float centerY, float radiusX, float radiusY, int numberOfSegments, const MeshContext& meshContext);
//	void DestroyMeshes(const VkDevice& device);
//private:
//	void Draw(const VkCommandBuffer& cmdBuffer) const;
//
//	std::vector<GP2Mesh<Vertex3D>> m_VecMeshes;
//	GP2Shader<Vertex3D> m_Shader;
//
//	void MakeRect(GP2Mesh<Vertex3D>& mesh, float top, float left, float bottom, float right) const;
//	void CreateRenderPass(const VulkanContext& vulkanContext, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);
//	void CreateGraphicsPipeline(const VkDevice& device);
//
//};
