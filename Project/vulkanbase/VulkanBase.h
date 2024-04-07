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
#include "GP2Shader.h"
#include "GP2Mesh.h"
#include "GP2Scene.h"
#include "GP2CommandPool.h"
#include "GP2CommandBuffer.h"
#include "GP2GraphicsPipeline.h"
#include "GP2DescriptorPool.h"
#include "Vertex.h"


const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase {
public:
	void run() {
		InitWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	void initVulkan() {
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
		m_Shader.Init(device, physicalDevice);
		m_Shader.CreateDescriptorSetLayout(device);
		m_Shader.CreateDescriptorSets(device);
		m_Pipeline.Initialize(device, swapChainImageFormat, FindDepthFormat(), m_Shader);
		
		// week 02

		m_CommandPool.Initialize(device, FindQueueFamilies(physicalDevice));

		m_Scene.AddRectangle(-0.95f, 0.25f, 0.15f, 0.75f, physicalDevice, device, m_CommandPool, graphicsQueue);
		//m_Scene.AddRoundedRectangle(-0.95f, -0.95f, 0.15f, 0.25f,0.3f,0.3f,10, physicalDevice, device, m_CommandPool, graphicsQueue);
		//m_Scene.AddOval(0, 0.5, .5f, 0.5f, 4, physicalDevice, device, m_CommandPool, graphicsQueue);

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

		m_Pipeline.Destroy(device);
		
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		m_CommandPool.DestroyCommandPool(device);

		m_Shader.DestroyUniformObjects(device);

		m_Scene.DestroyMeshes(device);

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

	GP2Shader m_Shader{"shaders/shader.vert.spv", "shaders/shader.frag.spv" };

	// Week 02
	// Queue families
	// CommandBuffer concept

	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	GP2CommandPool m_CommandPool{};
	GP2CommandBuffer m_CommandBuffer{};
	GP2Scene m_Scene{};

	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	std::vector<VkFramebuffer> swapChainFramebuffers;

	GP2GraphicsPipeline m_Pipeline{};
	
	void BeginRenderPass(const GP2CommandBuffer& cmdBuffer, VkFramebuffer currFrameBuffer, VkExtent2D extent);
	void EndRenderPass(const GP2CommandBuffer& cmdBuffer);
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
	bool HasStencilComponent(VkFormat format);
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	uint32_t FindMemoryType(uint32_t typeFilter, const VkMemoryPropertyFlags& properties) const;
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);


	// Week 05 
	// Logical and physical device

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void PickPhysicalDevice();
	bool IsDeviceSuitable(VkPhysicalDevice device);
	void CreateLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDevice device = VK_NULL_HANDLE;
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