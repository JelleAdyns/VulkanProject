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
		
		const std::string statueTexture{ "Resources/texture.jpg" };
		const std::string vehicleDiffuse{ "Resources/vehicle_diffuse.png"  };
		const std::string vehicleNormal{ "Resources/vehicle_normal.png"  };
		const std::string vehicleRoughness{ "Resources/vehicle_gloss.png"  };
		const std::string vehicleSpecular{ "Resources/vehicle_specular.png"  };
		const std::string boatDiffuse{ "Resources/T_Boat_Color.jpg" };
		const std::string sciFiDiffuse{ "Resources/TCom_Scifi_Floor3_1K_albedo.png" };
		const std::string sciFiNormal{ "Resources/TCom_Scifi_Floor3_1K_normal.png" };
		const std::string sciFiSpecular{ "Resources/TCom_Scifi_Floor3_1K_metallic.png" };
		const std::string sciFiRoughness{ "Resources/TCom_Scifi_Floor3_1K_roughness.png" };
		const std::string forrestDiffuse{ "Resources/xkglaihn_2K_Albedo.jpg" };
		const std::string forrestNormal{ "Resources/xkglaihn_2K_Normal.jpg" };
		const std::string forrestSpecular{ "Resources/xkglaihn_2K_Metalness.jpg" };
		const std::string forrestRoughness{ "Resources/xkglaihn_2K_Roughness.jpg" };
		const std::string metalDiffuse{ "Resources/vcenefcew_2K_Albedo.jpg" };
		const std::string metalNormal{ "Resources/vcenefcew_2K_Normal.jpg" };
		const std::string metalSpecular{ "Resources/vcenefcew_2K_Specular.jpg" };
		const std::string metalRoughness{ "Resources/vcenefcew_2K_Roughness.jpg" };

		m_pMapTextures[statueTexture] = std::make_unique<GP2Texture>(meshContext, statueTexture);
		m_pMapTextures[vehicleDiffuse] = std::make_unique<GP2Texture>(meshContext, vehicleDiffuse);
		m_pMapTextures[vehicleNormal] = std::make_unique<GP2Texture>(meshContext, vehicleNormal);
		m_pMapTextures[vehicleRoughness] = std::make_unique<GP2Texture>(meshContext, vehicleRoughness);
		m_pMapTextures[vehicleSpecular] = std::make_unique<GP2Texture>(meshContext, vehicleSpecular);
		m_pMapTextures[boatDiffuse] = std::make_unique<GP2Texture>(meshContext, boatDiffuse);
		m_pMapTextures[sciFiDiffuse] = std::make_unique<GP2Texture>(meshContext, sciFiDiffuse);
		m_pMapTextures[sciFiNormal] = std::make_unique<GP2Texture>(meshContext, sciFiNormal);
		m_pMapTextures[sciFiSpecular] = std::make_unique<GP2Texture>(meshContext, sciFiSpecular);
		m_pMapTextures[sciFiRoughness] = std::make_unique<GP2Texture>(meshContext, sciFiRoughness);
		m_pMapTextures[forrestDiffuse] = std::make_unique<GP2Texture>(meshContext, forrestDiffuse);
		m_pMapTextures[forrestNormal] = std::make_unique<GP2Texture>(meshContext, forrestNormal);
		m_pMapTextures[forrestSpecular] = std::make_unique<GP2Texture>(meshContext, forrestSpecular);
		m_pMapTextures[forrestRoughness] = std::make_unique<GP2Texture>(meshContext, forrestRoughness);
		m_pMapTextures[metalDiffuse] = std::make_unique<GP2Texture>(meshContext, metalDiffuse);
		m_pMapTextures[metalNormal] = std::make_unique<GP2Texture>(meshContext, metalNormal);
		m_pMapTextures[metalSpecular] = std::make_unique<GP2Texture>(meshContext, metalSpecular);
		m_pMapTextures[metalRoughness] = std::make_unique<GP2Texture>(meshContext, metalRoughness);


		m_PipelineDiffuse.AddGP2Mesh(CreateMesh("Resources/Boat.obj", meshContext));
		m_Pipeline3D.AddGP2Mesh(CreateSphere(glm::vec3{0.f,0.f, 0.f},100.f, 100, 100, meshContext));
		m_Pipeline3D.AddGP2Mesh(CreateCube(glm::vec3{0.f, 0.f, 0.f},100.f, meshContext));
		m_Pipeline3D.AddGP2Mesh(CreateMesh("Resources/vehicle.obj", meshContext));
		m_Pipeline3D.AddGP2Mesh(CreateMesh("Resources/birb.obj", meshContext));
		m_Pipeline3D.AddGP2Mesh(CreateSphere(glm::vec3{0.f,0.f, 0.f},50.f, 20, 20, meshContext));


		GP2Material* vehicleMaterial{ new GP2Material{} };
		vehicleMaterial->m_Diffuse = m_pMapTextures.at(vehicleDiffuse).get();
		vehicleMaterial->m_Normal = m_pMapTextures.at(vehicleNormal).get();
		vehicleMaterial->m_Roughness = m_pMapTextures.at(vehicleRoughness).get();
		vehicleMaterial->m_Specular = m_pMapTextures.at(vehicleSpecular).get();

		GP2Material* boatMaterial{ new GP2Material{} };
		boatMaterial->m_Diffuse = m_pMapTextures.at(boatDiffuse).get();
		boatMaterial->m_Normal = m_pMapTextures.at(vehicleNormal).get();
		boatMaterial->m_Roughness = m_pMapTextures.at(vehicleRoughness).get();
		boatMaterial->m_Specular = m_pMapTextures.at(vehicleSpecular).get();

		GP2Material* sphereMaterial{ new GP2Material{} };
		sphereMaterial->m_Diffuse = m_pMapTextures.at(sciFiDiffuse).get();
		sphereMaterial->m_Normal = m_pMapTextures.at(sciFiNormal).get();
		sphereMaterial->m_Roughness = m_pMapTextures.at(sciFiRoughness).get();
		sphereMaterial->m_Specular = m_pMapTextures.at(sciFiSpecular).get();

		GP2Material* beamMaterial{ new GP2Material{} };
		beamMaterial->m_Diffuse = m_pMapTextures.at(sciFiDiffuse).get();
		beamMaterial->m_Normal = m_pMapTextures.at(sciFiNormal).get();
		beamMaterial->m_Roughness = m_pMapTextures.at(sciFiRoughness).get();
		beamMaterial->m_Specular = m_pMapTextures.at(sciFiSpecular).get();

		GP2Material* metalMaterial{ new GP2Material{} };
		metalMaterial->m_Diffuse = m_pMapTextures.at(metalDiffuse).get();
		metalMaterial->m_Normal = m_pMapTextures.at(metalNormal).get();
		metalMaterial->m_Roughness = m_pMapTextures.at(metalRoughness).get();
		metalMaterial->m_Specular = m_pMapTextures.at(metalSpecular).get();

		GP2Material* birbMaterial{ new GP2Material{} };
		birbMaterial->m_Diffuse = m_pMapTextures.at(vehicleDiffuse).get();
		birbMaterial->m_Normal = m_pMapTextures.at(vehicleNormal).get();


		m_PipelineDiffuse.SetMaterial(boatMaterial,0);
		m_Pipeline3D.SetMaterial(sphereMaterial,0);
		m_Pipeline3D.SetMaterial(beamMaterial,1);
		m_Pipeline3D.SetMaterial(vehicleMaterial, 2);
		m_Pipeline3D.SetMaterial(vehicleMaterial,3);
		m_Pipeline3D.SetMaterial(metalMaterial,4);
		
		
		m_Pipeline2D.AddGP2Mesh(CreateRectangle(500, 20, HEIGHT, 300, meshContext));
		m_Pipeline2D.AddGP2Mesh(CreateRoundedRectangle(0, 1000, 200, WIDTH, 50.f, 50.f, 10, meshContext));
		m_Pipeline2D.AddGP2Mesh(CreateOval(WIDTH - 100.f, HEIGHT - 100.f, 100, 100, 40, meshContext));


		GP2Material* V2DMaterial{ new GP2Material{} };
		V2DMaterial->m_Diffuse = m_pMapTextures.at(forrestDiffuse).get();
		V2DMaterial->m_Normal = m_pMapTextures.at(statueTexture).get();
		V2DMaterial->m_Roughness = m_pMapTextures.at(statueTexture).get();
		V2DMaterial->m_Specular = m_pMapTextures.at(statueTexture).get();

		GP2Material* V2DMaterial2{ new GP2Material{} };
		V2DMaterial2->m_Diffuse = m_pMapTextures.at(statueTexture).get();
		V2DMaterial2->m_Normal = m_pMapTextures.at(statueTexture).get();
		V2DMaterial2->m_Roughness = m_pMapTextures.at(statueTexture).get();
		V2DMaterial2->m_Specular = m_pMapTextures.at(statueTexture).get();

		GP2Material* V2DMaterial3{ new GP2Material{} };
		V2DMaterial3->m_Diffuse = m_pMapTextures.at(statueTexture).get();
		V2DMaterial3->m_Normal = m_pMapTextures.at(statueTexture).get();
		V2DMaterial3->m_Roughness = m_pMapTextures.at(statueTexture).get();
		V2DMaterial3->m_Specular = m_pMapTextures.at(statueTexture).get();

		m_Pipeline2D.SetMaterial(V2DMaterial,0);
		m_Pipeline2D.SetMaterial(V2DMaterial2,1);
		m_Pipeline2D.SetMaterial(V2DMaterial3,2);

		m_PipelineDiffuse.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());
		m_Pipeline3D.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());
		m_Pipeline2D.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());

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