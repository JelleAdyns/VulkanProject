#include "vulkanbase/VulkanBase.h"

void VulkanBase::InitWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->GetCamera()->KeyEvent(key, scancode, action, mods);
		});
	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->GetCamera()->MouseMove(window, xpos, ypos);
		});
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		void* pUser = glfwGetWindowUserPointer(window);
		VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
		vBase->GetCamera()->MouseEvent(window, button, action, mods);
		});
}

void VulkanBase::InitializeMaterials(const MeshContext& meshContext)
{
	const std::string statueTexture{ "Resources/texture.jpg" };
	const std::string vehicleDiffuse{ "Resources/vehicle_diffuse.png" };
	const std::string vehicleNormal{ "Resources/vehicle_normal.png" };
	const std::string vehicleRoughness{ "Resources/vehicle_gloss.png" };
	const std::string vehicleSpecular{ "Resources/vehicle_specular.png" };
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


	const std::string vehicleMatKey{ "Vehicle" };
	const std::string boatMatKey{ "Boat" };
	const std::string sciFiMatKey{ "SciFi" };
	const std::string roughMetalMatKey{ "RoughMetal" };
	const std::string forrestMatKey{ "Forrest" };
	const std::string statueMatKey{ "Statue" };


	GP2Material* vehicleMaterial{ new GP2Material{} };
	vehicleMaterial->m_Diffuse = m_pMapTextures.at(vehicleDiffuse).get();
	vehicleMaterial->m_Normal = m_pMapTextures.at(vehicleNormal).get();
	vehicleMaterial->m_Roughness = m_pMapTextures.at(vehicleRoughness).get();
	vehicleMaterial->m_Specular = m_pMapTextures.at(vehicleSpecular).get();

	m_pMapMaterials[vehicleMatKey] = std::make_unique<GP2Material>();
	m_pMapMaterials[vehicleMatKey].reset(vehicleMaterial);

	GP2Material* boatMaterial{ new GP2Material{} };
	boatMaterial->m_Diffuse = m_pMapTextures.at(boatDiffuse).get();
	boatMaterial->m_Normal = m_pMapTextures.at(vehicleNormal).get();
	boatMaterial->m_Roughness = m_pMapTextures.at(vehicleRoughness).get();
	boatMaterial->m_Specular = m_pMapTextures.at(vehicleSpecular).get();

	m_pMapMaterials[boatMatKey] = std::make_unique<GP2Material>();
	m_pMapMaterials[boatMatKey].reset(boatMaterial);

	GP2Material* sciFiMaterial{ new GP2Material{} };
	sciFiMaterial->m_Diffuse = m_pMapTextures.at(sciFiDiffuse).get();
	sciFiMaterial->m_Normal = m_pMapTextures.at(sciFiNormal).get();
	sciFiMaterial->m_Roughness = m_pMapTextures.at(sciFiRoughness).get();
	sciFiMaterial->m_Specular = m_pMapTextures.at(sciFiSpecular).get();

	m_pMapMaterials[sciFiMatKey] = std::make_unique<GP2Material>();
	m_pMapMaterials[sciFiMatKey].reset(sciFiMaterial);

	GP2Material* roughMetalMaterial{ new GP2Material{} };
	roughMetalMaterial->m_Diffuse = m_pMapTextures.at(metalDiffuse).get();
	roughMetalMaterial->m_Normal = m_pMapTextures.at(metalNormal).get();
	roughMetalMaterial->m_Roughness = m_pMapTextures.at(metalRoughness).get();
	roughMetalMaterial->m_Specular = m_pMapTextures.at(metalSpecular).get();

	m_pMapMaterials[roughMetalMatKey] = std::make_unique<GP2Material>();
	m_pMapMaterials[roughMetalMatKey].reset(roughMetalMaterial);



	GP2Material* forrestMaterial{ new GP2Material{} };
	forrestMaterial->m_Diffuse = m_pMapTextures.at(forrestDiffuse).get();
	forrestMaterial->m_Normal = m_pMapTextures.at(statueTexture).get();
	forrestMaterial->m_Roughness = m_pMapTextures.at(statueTexture).get();
	forrestMaterial->m_Specular = m_pMapTextures.at(statueTexture).get();

	m_pMapMaterials[forrestMatKey] = std::make_unique<GP2Material>();
	m_pMapMaterials[forrestMatKey].reset(forrestMaterial);

	GP2Material* statueMaterial{ new GP2Material{} };
	statueMaterial->m_Diffuse = m_pMapTextures.at(statueTexture).get();
	statueMaterial->m_Normal = m_pMapTextures.at(statueTexture).get();
	statueMaterial->m_Roughness = m_pMapTextures.at(statueTexture).get();
	statueMaterial->m_Specular = m_pMapTextures.at(statueTexture).get();

	m_pMapMaterials[statueMatKey] = std::make_unique<GP2Material>();
	m_pMapMaterials[statueMatKey].reset(statueMaterial);

}

