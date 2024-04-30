#include "GP2Texture.h"
#include "GP2Buffer.h"
#include "vulkanbase/vulkanBase.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void GP2Texture::CreateTextureImage(const MeshContext& meshContext)
{
    int texWidth{}, texHeight{}, texChannels{};
    stbi_uc* pixels = stbi_load("Resources/texture2.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) throw std::runtime_error("failed to load texture image!");

    GP2Buffer stagingBuffer{};

    stagingBuffer.CreateBuffer(meshContext.device, meshContext.physicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(meshContext.device, stagingBuffer.GetVkBufferMemory(), 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(meshContext.device, stagingBuffer.GetVkBufferMemory());

    stbi_image_free(pixels); //TODO: look for a better use of your member vars

    VulkanBase::CreateImage(meshContext.device, meshContext.physicalDevice,
        texWidth, texHeight,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_Image, m_DeviceMemory);

    VulkanBase::TransitionImageLayout(meshContext, m_Image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    CopyBufferToImage(meshContext, stagingBuffer.GetVkBuffer(), m_Image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

    VulkanBase::TransitionImageLayout(meshContext, m_Image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    stagingBuffer.DestroyBuffer(meshContext.device);
}

void GP2Texture::DestroyTexture(const VkDevice& device)
{
    vkDestroyImage(device, m_Image, nullptr);
    vkFreeMemory(device, m_DeviceMemory, nullptr);
}

void GP2Texture::CopyBufferToImage(const MeshContext& meshContext, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    GP2CommandBuffer cmdBuffer = meshContext.commandPool.CreateCommandBuffer(meshContext.device);

    cmdBuffer.BeginSingleTimeCommands();


    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {width, height, 1};

    vkCmdCopyBufferToImage(
        cmdBuffer.GetVkCommandBuffer(),
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    cmdBuffer.EndSingleTimeCommands(meshContext);
}