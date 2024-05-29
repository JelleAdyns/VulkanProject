#include "vulkanbase/VulkanBase.h"
#include "nlohmann/json.hpp"

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

void VulkanBase::InitializeScene(const VulkanContext& vulkanContext, const MeshContext& meshContext)
{
	const std::string resourcePath{ "Resources/" };

	if (std::ifstream jsonMaterials{ "Resources/Materials.json" }; jsonMaterials.is_open())
	{
		nlohmann::json jsonData = nlohmann::json::parse(jsonMaterials);

		int amountOfMAterials{jsonData["AmountOfMaterials"].get<int>()};

		for (int materialIndex = 0; materialIndex < amountOfMAterials; ++materialIndex)
		{
			auto materialObject = (jsonData["Materials"])[materialIndex];

			const std::string diffuseFile = materialObject["Diffuse"].get<std::string>();
			const std::string normalsFile = materialObject["Normals"].get<std::string>();
			const std::string specularFile = materialObject["Specular"].get<std::string>();
			const std::string roughnessFile = materialObject["Roughness"].get<std::string>();

			if (auto it = m_pMapTextures.find(diffuseFile); it == m_pMapTextures.cend())
			{
				m_pMapTextures[diffuseFile] = std::make_unique<GP2Texture>(meshContext, resourcePath + diffuseFile);
			}
			if (auto it = m_pMapTextures.find(normalsFile); it == m_pMapTextures.cend())
			{
				m_pMapTextures[normalsFile] = std::make_unique<GP2Texture>(meshContext, resourcePath + normalsFile);
			}
			if (auto it = m_pMapTextures.find(specularFile); it == m_pMapTextures.cend())
			{
				m_pMapTextures[specularFile] = std::make_unique<GP2Texture>(meshContext, resourcePath + specularFile);
			}
			if (auto it = m_pMapTextures.find(roughnessFile); it == m_pMapTextures.cend())
			{
				m_pMapTextures[roughnessFile] = std::make_unique<GP2Texture>(meshContext, resourcePath + roughnessFile);
			}

			GP2Material* material{ new GP2Material{} };
			material->m_Diffuse = m_pMapTextures.at(diffuseFile).get();
			material->m_Normal = m_pMapTextures.at(normalsFile).get();
			material->m_Roughness = m_pMapTextures.at(roughnessFile).get();
			material->m_Specular = m_pMapTextures.at(specularFile).get();

			m_pMapMaterials[materialObject["Key"].get<std::string>()].reset(material);

		}
		
	}


	m_PipelineDiffuse.AddGP2Mesh(CreateMesh("Resources/Boat.obj", meshContext));

	m_PipelineDiffuse.SetMaterial(m_pMapMaterials["Boat"].get(), 0);


	m_Pipeline3D.AddGP2Mesh(CreateSphere(glm::vec3{ 0.f,0.f, 0.f }, 100.f, 100, 100, meshContext));
	m_Pipeline3D.AddGP2Mesh(CreateCube(glm::vec3{ 0.f, 0.f, 0.f }, 100.f, meshContext));
	m_Pipeline3D.AddGP2Mesh(CreateMesh("Resources/vehicle.obj", meshContext));
	m_Pipeline3D.AddGP2Mesh(CreateMesh("Resources/birb.obj", meshContext));
	m_Pipeline3D.AddGP2Mesh(CreateSphere(glm::vec3{ 0.f,0.f, 0.f }, 50.f, 100, 100, meshContext));

	m_Pipeline3D.SetMaterial(m_pMapMaterials["SciFi"].get(), 0);
	m_Pipeline3D.SetMaterial(m_pMapMaterials["SciFi"].get(), 1);
	m_Pipeline3D.SetMaterial(m_pMapMaterials["Vehicle"].get(), 2);
	m_Pipeline3D.SetMaterial(m_pMapMaterials["Vehicle"].get(), 3);
	m_Pipeline3D.SetMaterial(m_pMapMaterials["RoughMetal"].get(), 4);


	m_Pipeline2D.AddGP2Mesh(CreateRectangle(500, 20, HEIGHT, 300, meshContext));
	m_Pipeline2D.AddGP2Mesh(CreateRoundedRectangle(0, 1000, 200, WIDTH, 50.f, 50.f, 10, meshContext));
	m_Pipeline2D.AddGP2Mesh(CreateOval(WIDTH - 100.f, HEIGHT - 100.f, 100, 100, 40, meshContext));

	m_Pipeline2D.SetMaterial(m_pMapMaterials["Forrest"].get(), 0);
	m_Pipeline2D.SetMaterial(m_pMapMaterials["Statue"].get(), 1);
	m_Pipeline2D.SetMaterial(m_pMapMaterials["Statue"].get(), 2);


	m_PipelineDiffuse.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());
	m_Pipeline3D.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());
	m_Pipeline2D.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());

}

