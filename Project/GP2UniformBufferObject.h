#pragma once
#include "vulkanbase/VulkanUtil.h"
#include "ContextStructs.h"
#include "GP2Buffer.h"

template < typename UBO >
class GP2UniformBufferObject
{
public:

	GP2UniformBufferObject() = default;
	~GP2UniformBufferObject() = default;

	GP2UniformBufferObject(const GP2UniformBufferObject& other) = delete;
	GP2UniformBufferObject(GP2UniformBufferObject&& other) noexcept = delete;
	GP2UniformBufferObject& operator=(const GP2UniformBufferObject& other) = delete;
	GP2UniformBufferObject& operator=(GP2UniformBufferObject&& other) noexcept = delete;

	void DestroyBuffer(const VkDevice& device) { m_UBOBuffer.DestroyBuffer(device); }

	void Initialize(const VulkanContext & context);
	void Upload();
	void SetData(UBO ubo) { m_UBOSrc = ubo;}
	
	VkBuffer GetVkBuffer() { return m_UBOBuffer.GetVkBuffer(); }
 private:
	 GP2Buffer m_UBOBuffer{};
	 UBO m_UBOSrc{};
 };

template < typename UBO >
void GP2UniformBufferObject<UBO>::Initialize(const VulkanContext& context)
{
	m_UBOBuffer.CreateBuffer(context.device, context.physicalDevice, sizeof(UBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	m_UBOBuffer.Map(context.device);
}

template < typename UBO >
void GP2UniformBufferObject<UBO>::Upload()
{
	m_UBOBuffer.Upload(sizeof(UBO), &m_UBOSrc);
}