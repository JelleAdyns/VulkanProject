#include "vulkanbase/VulkanBase.h"
#include "nlohmann/json.hpp"

bool VulkanBase::m_Rotate{ true };

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

	if (std::ifstream jsonMaterials{ "Resources/Materials.json" }; jsonMaterials.is_open())
	{
		nlohmann::json jsonData = nlohmann::json::parse(jsonMaterials);

		InitializeMaterials(jsonData["Materials"], meshContext);
	}

	if (std::ifstream jsonScene{ "Resources/Scene.json" }; jsonScene.is_open())
	{
		nlohmann::json jsonData = nlohmann::json::parse(jsonScene);
		auto scene = jsonData["Scene"];

		Initialize3DMeshes(scene["PBRMeshes"], m_Pipeline3D, meshContext);

		Initialize3DMeshes(scene["FlatShadingMeshes"], m_PipelineDiffuse, meshContext);

		Initialize2DMeshes(scene["2DMeshes"], m_Pipeline2D, meshContext);
	}


	m_PipelineDiffuse.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());
	m_Pipeline3D.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());
	m_Pipeline2D.Initialize(vulkanContext, meshContext, swapChainImageFormat, FindDepthFormat());

}

void VulkanBase::InitializeMaterials(const nlohmann::json& materials, const MeshContext& meshContext)
{
	for (auto materialObject : materials)
	{
		const std::string diffuseFile = materialObject["Diffuse"].get<std::string>();
		const std::string normalsFile = materialObject["Normals"].get<std::string>();
		const std::string specularFile = materialObject["Specular"].get<std::string>();
		const std::string roughnessFile = materialObject["Roughness"].get<std::string>();

		if (auto it = m_pMapTextures.find(diffuseFile); it == m_pMapTextures.cend())
		{
			m_pMapTextures[diffuseFile] = std::make_unique<GP2Texture>(meshContext, m_ResourcePath + diffuseFile);
		}
		if (auto it = m_pMapTextures.find(normalsFile); it == m_pMapTextures.cend())
		{
			m_pMapTextures[normalsFile] = std::make_unique<GP2Texture>(meshContext, m_ResourcePath + normalsFile);
		}
		if (auto it = m_pMapTextures.find(specularFile); it == m_pMapTextures.cend())
		{
			m_pMapTextures[specularFile] = std::make_unique<GP2Texture>(meshContext, m_ResourcePath + specularFile);
		}
		if (auto it = m_pMapTextures.find(roughnessFile); it == m_pMapTextures.cend())
		{
			m_pMapTextures[roughnessFile] = std::make_unique<GP2Texture>(meshContext, m_ResourcePath + roughnessFile);
		}

		GP2Material* material{ new GP2Material{} };
		material->m_Diffuse = m_pMapTextures.at(diffuseFile).get();
		material->m_Normal = m_pMapTextures.at(normalsFile).get();
		material->m_Roughness = m_pMapTextures.at(roughnessFile).get();
		material->m_Specular = m_pMapTextures.at(specularFile).get();

		m_pMapMaterials[materialObject["Key"].get<std::string>()].reset(material);

	}
}

void VulkanBase::Initialize3DMeshes(const nlohmann::json& meshes, GP2GraphicsPipeline<Vertex3D>& pipeline, const MeshContext& meshContext)
{

	for (int meshIndex = 0; meshIndex < meshes.size(); ++meshIndex)
	{
		auto mesh = meshes.at(meshIndex);

		if (auto pos = mesh.find("ObjFile"); pos != mesh.cend())
		{
			auto filename = mesh["ObjFile"].get<std::string>();
			pipeline.AddGP2Mesh(CreateMesh(m_ResourcePath + filename, meshContext));
		}
		if (auto pos = mesh.find("Cube"); pos != mesh.cend())
		{
			auto size = mesh["Cube"].get<float>();
			pipeline.AddGP2Mesh(CreateCube(size, meshContext));
		}
		if (auto pos = mesh.find("Beam"); pos != mesh.cend())
		{
			auto beam = mesh["Beam"];
			auto width = beam["Width"].get<float>();
			auto height = beam["Height"].get<float>();
			auto depth = beam["Depth"].get<float>();
			pipeline.AddGP2Mesh(CreateBeam(width, height, depth, meshContext));
		}
		if (auto pos = mesh.find("Sphere"); pos != mesh.cend())
		{
			auto sphere = mesh["Sphere"];
			auto Radius = sphere["Radius"].get<float>();
			auto xDivisions = sphere["xDivisions"].get<int>();
			auto yDivisions = sphere["yDivisions"].get<int>();
			pipeline.AddGP2Mesh(CreateSphere(Radius, xDivisions, yDivisions, meshContext));
		}

		auto materialKey = mesh["Material"].get<std::string>();
		pipeline.SetMaterial(m_pMapMaterials[materialKey].get(), meshIndex);


		auto translation = mesh["Translation"];
		glm::vec3 pos{};
		pos.x = translation["x"].get<float>();
		pos.y = translation["y"].get<float>();
		pos.z = translation["z"].get<float>();
		pipeline.SetMeshTranslation(pos, meshIndex);

	}
}

void VulkanBase::Initialize2DMeshes(const nlohmann::json& meshes, GP2GraphicsPipeline<Vertex2D>& pipeline, const MeshContext& meshContext)
{

	for (int meshIndex = 0; meshIndex < meshes.size(); ++meshIndex)
	{
		auto mesh = meshes.at(meshIndex);

		if (auto pos = mesh.find("Rect"); pos != mesh.cend())
		{
			auto rect = mesh["Rect"];
			auto top = rect["Top"].get<float>();
			auto left = rect["Left"].get<float>();
			auto bottom = rect["Bottom"].get<float>();
			auto right = rect["Right"].get<float>();
			pipeline.AddGP2Mesh(CreateRectangle(top, left, bottom, right, meshContext));
		}
		if (auto pos = mesh.find("RoundedRect"); pos != mesh.cend())
		{
			auto rect = mesh["RoundedRect"];
			auto top = rect["Top"].get<float>();
			auto left = rect["Left"].get<float>();
			auto bottom = rect["Bottom"].get<float>();
			auto right = rect["Right"].get<float>();
			auto radiusX = rect["RadiusX"].get<float>();
			auto radiusY = rect["RadiusY"].get<float>();
			auto segments = rect["SegmentsPerCorner"].get<int>();
			pipeline.AddGP2Mesh(CreateRoundedRectangle(top, left, bottom, right, radiusX, radiusY, segments, meshContext));
		}
		if (auto pos = mesh.find("Oval"); pos != mesh.cend())
		{
			auto oval = mesh["Oval"];
			auto x = oval["x"].get<float>();
			auto y = oval["y"].get<float>();
			auto radiusX = oval["RadiusX"].get<float>();
			auto radiusY = oval["RadiusY"].get<float>();
			auto segments = oval["Segments"].get<int>();
			pipeline.AddGP2Mesh(CreateOval(x, y, radiusX, radiusY, segments, meshContext));
		}

		auto materialKey = mesh["Material"].get<std::string>();
		pipeline.SetMaterial(m_pMapMaterials[materialKey].get(), meshIndex);
	}
}

