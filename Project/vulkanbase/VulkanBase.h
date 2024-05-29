#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include "GP2Mesh.h"
#include "GP2CommandPool.h"
#include "GP2CommandBuffer.h"
#include "GP2GraphicsPipeline.h"
#include "GP2DescriptorPool.h"
#include "GP2Texture.h"
#include "Vertex.h"
#include "Camera.h"
#include "ContextStructs.h"
#include <map>


const std::vector<const char*> validationLayers = 
{
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = 
{
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices 
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails 
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase 
{
public:
	void run() 
	{
		InitWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	Camera* GetCamera() { return m_Camera.get(); }
	static uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, const VkMemoryPropertyFlags& properties);
	static void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	static VkImageView CreateImageView(const VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	static void TransitionImageLayout(const MeshContext& meshContext, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	static bool HasStencilComponent(VkFormat format);

private:
	void initVulkan() 
	{
		
		// week 06
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();

		// week 05
		PickPhysicalDevice();
		CreateLogicalDevice();

		// week 04 
		CreateSwapChain();
		CreateImageViews();
		
		// week 03
		CreateRenderPass(device, swapChainImageFormat, FindDepthFormat());
		MeshContext meshContext
		{
			device,
			physicalDevice,
			m_CommandPool,
			graphicsQueue
		};
		VulkanContext vulkanContext
		{
			device,
			physicalDevice,
			m_RenderPass,
			swapChainExtent
		};
		m_CommandPool.Initialize(device, FindQueueFamilies(physicalDevice));
	
		
		// week 02
		m_Camera->Initialize(WIDTH, HEIGHT, 45.f, {0.f,0.f,-150.f});

		InitializeScene(vulkanContext, meshContext);

		m_CommandBuffer = m_CommandPool.CreateCommandBuffer(device);
		CreateDepthResources();
		CreateFrameBuffers();

		// week 06
		CreateSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			DrawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup()
	{
		CleanupSwapChain();

		m_PipelineDiffuse.Destroy(device);
		m_Pipeline3D.Destroy(device);
		m_Pipeline2D.Destroy(device);
		vkDestroyRenderPass(device, m_RenderPass, nullptr);

		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		m_CommandPool.DestroyCommandPool(device);

		//m_Shader.DestroyUniformObjects(device);

		m_PipelineDiffuse.DestroyMeshes(device);
		m_Pipeline3D.DestroyMeshes(device);
		m_Pipeline2D.DestroyMeshes(device);
		for (auto& texturePair : m_pMapTextures)
		{
			texturePair.second->DestroyTexture(device);
		}
		
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void CreateSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void InitWindow();
	void InitializeScene(const VulkanContext& vulkanContext, const MeshContext& meshContext);
	std::unique_ptr<Camera> m_Camera{std::make_unique<Camera>()};

	//GP2Shader3D m_Shader{"shaders/objshader.vert.spv", "shaders/objshader.frag.spv" };

	// Week 02
	// Queue families
	// CommandBuffer concept
	
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	GP2CommandPool m_CommandPool{};
	GP2CommandBuffer m_CommandBuffer{};

	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkRenderPass m_RenderPass{};

	GP2GraphicsPipeline<Vertex3D> m_PipelineDiffuse{ "shaders/objshader.vert.spv", "shaders/objshader.frag.spv"};
	GP2GraphicsPipeline<Vertex3D> m_Pipeline3D{ "shaders/PBRshader.vert.spv", "shaders/PBRshader.frag.spv"};
	GP2GraphicsPipeline<Vertex2D> m_Pipeline2D{ "shaders/shader.vert.spv", "shaders/shader.frag.spv"};

	std::map<std::string, std::unique_ptr<GP2Texture>> m_pMapTextures;
	std::map<std::string, std::unique_ptr<GP2Material>> m_pMapMaterials;

	void BeginRenderPass(const GP2CommandBuffer& cmdBuffer, VkFramebuffer currFrameBuffer, VkExtent2D extent);
	void EndRenderPass(const GP2CommandBuffer& cmdBuffer);
	void CreateRenderPass(const VkDevice& device, const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);

	void CreateFrameBuffers();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void CreateSwapChain();
	void CreateImageViews();
	void RecreateSwapChain();
	void CleanupSwapChain();
	std::vector<VkImageView> swapChainImageViews;

	//Depth-Stencil
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	void CreateDepthResources();
	VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat FindDepthFormat();


	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	void CreateLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device;
	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void SetupDebugMessenger();
	std::vector<const char*> GetRequiredExtensions();
	bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
	void CreateInstance();

	void CreateSyncObjects();
	void DrawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};
//VkDevice VulkanBase::device{ VK_NULL_HANDLE };
//VkPhysicalDevice VulkanBase::physicalDevice{ VK_NULL_HANDLE };