#pragma once
#include <cassert>
#include <iostream>
#include "Math.h"
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi
#include <GLFW/glfw3.h>

class Camera final
{
public:
	Camera() = default;

	Camera(const glm::vec3& _origin, float _fovAngle);

	void Initialize(int _width, int _height, float _fovAngle = 90.f, glm::vec3 _origin = { 0.f,0.f,0.f });


	void Update();

	void HandleKeyMovement(const uint8_t* keys, float elapsedSec);
	void TransformForwardVector();

	void KeyEvent(int key, int scancode, int action, int mods);
	void MouseMove(GLFWwindow* window, double xpos, double ypos);
	void MouseEvent(GLFWwindow* window, int button, int action, int mods);

	const glm::mat4x4& GetViewMatrix() const { return m_ViewMatrix; }
	const glm::mat4x4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
	const glm::vec3& GetCameraOrigin() const { return m_Origin; }
private:

	void CalculateViewMatrix();
	void CalculateProjectionMatrix();

	const float m_Near{ .1f };
	const float m_Far{ 1000.f };

	glm::vec3 m_Origin{};
	float m_FovAngle{ 90.f };
	float m_Fov{ glm::tan((m_FovAngle * (glm::pi<float>() / 180.f)) / 2.f) };

	glm::vec3 m_Forward{ glm::vec3{0,0,1} };
	glm::vec3 m_Up{ glm::vec3{0,1,0} };
	glm::vec3 m_Right{ glm::vec3{1,0,0} };

	float m_TotalPitch{};
	float m_TotalYaw{};
	float m_Radius{};
	float m_Rotation{};
	glm::vec2 m_DragStart{};

	const float m_TranslateSpeed{ 30.f };

	glm::mat4x4 m_InvViewMatrix{};
	glm::mat4x4 m_ViewMatrix{};
	glm::mat4x4 m_ProjectionMatrix{};

	int m_Width{};
	int m_Height{};
};



