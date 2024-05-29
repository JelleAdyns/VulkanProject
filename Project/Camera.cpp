#include "Camera.h"

RenderProperties Camera::m_RenderProperties{ RenderMode::Combined, true };

Camera::Camera(const glm::vec3& _origin, float _fovAngle) :
	m_Origin{ _origin },
	m_FovAngle{ _fovAngle }
{

}

void Camera::Initialize(int _width, int _height, float _fovAngle, glm::vec3 _origin)
{
	m_FovAngle = _fovAngle;
	m_Fov = glm::tan((m_FovAngle * (glm::pi<float>() / 180.f)) / 2.f);

	m_Origin = _origin;

	m_Width = _width;
	m_Height = _height;
	CalculateViewMatrix();
	CalculateProjectionMatrix();
}

void Camera::CalculateViewMatrix()
{
	glm::mat4 finalRotation = glm::rotate(glm::mat4x4(1.f), glm::radians(m_TotalYaw), m_UnitY);
	finalRotation = glm::rotate(finalRotation, glm::radians(m_TotalPitch), glm::vec3{ 1,0,0 });

	m_Forward = glm::normalize(finalRotation[2]);

	m_Right = glm::normalize(glm::cross(m_Forward, m_UnitY));
	m_ViewMatrix = glm::lookAt(m_Origin, m_Origin + m_Forward, m_UnitY);
}

void Camera::CalculateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspectiveFov(m_Fov, static_cast<float>(m_Width), static_cast<float>(m_Height), m_Near, m_Far);
}

void Camera::KeyEvent(int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
	{
		switch (m_RenderProperties.renderingMode)
		{
		case RenderMode::Combined:
			m_RenderProperties.renderingMode = RenderMode::Diffuse;
			break;
		case RenderMode::Diffuse:
			m_RenderProperties.renderingMode = RenderMode::ObservedArea;
			break;
		case RenderMode::ObservedArea:
			m_RenderProperties.renderingMode = RenderMode::Specular;
			break;
		case RenderMode::Specular:
			m_RenderProperties.renderingMode = RenderMode::Combined;
			break;
		}
	}
	if (key == GLFW_KEY_F2 && action == GLFW_PRESS)
	{
		if (m_RenderProperties.useNormalMap) m_RenderProperties.useNormalMap = 0;
		else m_RenderProperties.useNormalMap = 1;
	}

	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS)) m_Origin += m_Forward * m_TranslateSpeed;
	
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS)) m_Origin -= m_Forward * m_TranslateSpeed;
	
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS)) m_Origin -= m_Right * m_TranslateSpeed;
	
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS)) m_Origin += m_Right * m_TranslateSpeed;

	if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS)) m_Origin -= m_UnitY * m_TranslateSpeed;
	
	if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS)) m_Origin += m_UnitY * m_TranslateSpeed;
	
	if (key == GLFW_KEY_Z && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		++m_FovAngle;
		if (m_FovAngle >= 145) m_FovAngle = 145;
		m_Fov = glm::tan(glm::radians(m_FovAngle / 2));
		CalculateProjectionMatrix();
		std::cout << m_FovAngle << std::endl;
	}
	if (key == GLFW_KEY_C && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		--m_FovAngle;
		if (m_FovAngle < 1) m_FovAngle = 1;
		m_Fov = glm::tan( glm::radians( m_FovAngle / 2));
		CalculateProjectionMatrix();
		std::cout << m_FovAngle << std::endl;
	}
	CalculateViewMatrix();
}
void Camera::MouseMove(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		float dx = static_cast<float>(xpos) - m_DragStart.x;
		if (dx > 0)
		{
			m_TotalYaw -= 1;
			m_DragStart.x = static_cast<float>(xpos);
		}

		else if (dx < 0)
		{
			m_TotalYaw += 1;
			m_DragStart.x = static_cast<float>(xpos);
		}

		float dy = static_cast<float>(ypos) - m_DragStart.y;
		if (dy > 0)
		{
			m_TotalPitch += 1;
			m_DragStart.y = static_cast<float>(ypos);
		}
		else if (dy < 0)
		{
			m_TotalPitch -= 1;
			m_DragStart.y = static_cast<float>(ypos);
		}

		if (m_TotalPitch >= 89) m_TotalPitch = 89;
		if (m_TotalPitch <= -89) m_TotalPitch = -89;
		CalculateViewMatrix();
	}
}
void Camera::MouseEvent(GLFWwindow* window, int button, int action, int mods)
{
}


