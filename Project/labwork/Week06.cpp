#include "vulkanbase/VulkanBase.h"
#include <chrono>
#include <thread>

void VulkanBase::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}


void VulkanBase::SetupDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	PopulateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("failed to set up debug messenger!");
	}
}

void VulkanBase::CreateSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to create synchronization objects for a frame!");
	}

}

void VulkanBase::DrawFrame() {
	vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &inFlightFence);

	uint32_t imageIndex{};
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	
	vkResetFences(device, 1, &inFlightFence);

	m_CommandBuffer.Reset();
	m_CommandBuffer.BeginRecordBuffer();

	BeginRenderPass(m_CommandBuffer, swapChainFramebuffers[imageIndex], swapChainExtent);


	glm::mat4 vehicle{ 1.0f };
	glm::mat4 boat{ 1.0f };
	glm::mat4 orb{ 1.0f };
	glm::mat4 beam{ 1.0f };
	glm::mat4 metal{ 1.0f };
	glm::mat4 forrest{ 1.0f };
	glm::mat4 birb{1.0f};

	if(m_Rotate)
	{
		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


		vehicle = glm::rotate(vehicle, time * glm::radians(45.f), glm::vec3(0.0f, 1.0f, 0.0f));
		orb = glm::rotate(orb, time * glm::radians(45.f), glm::vec3(0.0f, 1.0f, 0.0f));
		beam = glm::rotate(beam, time * glm::radians(45.f), glm::vec3(0.0f, 1.0f, 0.0f));
		metal = glm::rotate(metal, time * glm::radians(20.f), glm::vec3(0.0f, .0f, 01.0f));
		forrest = glm::rotate(forrest, time * glm::radians(20.f), glm::vec3(0.0f, 1.0f, 0.0f));
		birb = glm::rotate(birb, time * glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));

	

		m_Pipeline3D.UpdateMeshMatrix(vehicle, 0);
		m_Pipeline3D.UpdateMeshMatrix(vehicle, 1);
		m_Pipeline3D.UpdateMeshMatrix(orb, 2);
		m_Pipeline3D.UpdateMeshMatrix(orb, 3);
		m_Pipeline3D.UpdateMeshMatrix(metal, 4);
		m_Pipeline3D.UpdateMeshMatrix(forrest, 5);
		m_Pipeline3D.UpdateMeshMatrix(forrest, 6);
	}

	boat = glm::scale(boat, glm::vec3(0.25f, 0.25f, 0.25f));
	m_PipelineDiffuse.UpdateMeshMatrix(boat, 0);

	m_PipelineDiffuse.UpdateUniformBuffer(m_Camera->GetViewMatrix(), m_Camera->GetProjectionMatrix());
	m_PipelineDiffuse.Record(m_CommandBuffer, swapChainExtent);

	
	m_Pipeline3D.UpdateUniformBuffer(m_Camera->GetViewMatrix(), m_Camera->GetProjectionMatrix());
	m_Pipeline3D.Record(m_CommandBuffer, swapChainExtent);


	ViewProjection vp{};
	glm::vec3 scaleFactors(1.f/WIDTH , 1.f/HEIGHT, 1.0f);
	vp.view = glm::scale(vp.view, scaleFactors);
	vp.view = glm::translate(vp.view, glm::vec3(-float(WIDTH), -float(HEIGHT), 0.0f));
	vp.view = glm::scale(vp.view, glm::vec3(2.f, 2.f, 1.0f));
	
	m_Pipeline2D.UpdateUniformBuffer(vp.view, vp.proj);
	m_Pipeline2D.Record(m_CommandBuffer, swapChainExtent);

	EndRenderPass(m_CommandBuffer);
	m_CommandBuffer.EndRecordBuffer();

	//m_Camera.
	//m_Shader.UpdateUniformBuffer(imageIndex, (float)swapChainExtent.width / (float)swapChainExtent.height, 45.f);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	m_CommandBuffer.Submit(submitInfo);

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		RecreateSwapChain();
	}


	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

bool CheckValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

std::vector<const char*> VulkanBase::GetRequiredExtensions() {
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool VulkanBase::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void VulkanBase::CreateInstance() {
	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}