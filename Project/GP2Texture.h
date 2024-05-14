#pragma once

#include "vulkanbase/VulkanUtil.h"
#include "ContextStructs.h"

class GP2Texture
{
public:

	GP2Texture(const MeshContext& meshContext, const std::string& filename):
		m_TextureFile{filename}
	{
		CreateTextureImage(meshContext);
	};
	~GP2Texture() = default;

	GP2Texture(const GP2Texture& other) = default;
	GP2Texture(GP2Texture&& other) noexcept = default;
	GP2Texture& operator=(const GP2Texture& other) = default;
	GP2Texture& operator=(GP2Texture&& other) noexcept = default;

	void DestroyTexture(const VkDevice& device);

	const VkImageView& GetImageView() const;
	const VkSampler& GetSampler() const;
private:

	void CreateTextureImage(const MeshContext& meshContext);
	void CreateTextureSampler(const MeshContext& meshContext);
	void CopyBufferToImage(const MeshContext& meshContext, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	std::string m_TextureFile;

	VkImage m_Image{};
	VkImageView m_ImageView{};
	VkDeviceMemory m_DeviceMemory{};
	VkSampler m_Sampler{};
};
