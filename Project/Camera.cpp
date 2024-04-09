#include "Camera.h"

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
	m_Right = glm::normalize(glm::cross(glm::vec3{ 0,1,0 }, m_Forward));
	/*m_Up = glm::normadslize(glm::cross(m_Forward, m_Right));

	glm::mat4x4 invView{ {m_Right, 0},
					{m_Up, 0},
					{m_Forward, 0},
					{m_Origin, 1} };
	m_InvViewMatrix = invView;
	m_ViewMatrix = glm::inverse(invView);*/

	m_ViewMatrix = glm::lookAtLH(m_Origin, m_Origin + m_Forward, m_Up);
	//m_ViewMatrix = glm::lookAtLH(m_Origin, glm::vec3{0.f,0.f,0.f}, m_Up);
}

void Camera::CalculateProjectionMatrix()
{
	m_ProjectionMatrix = glm::perspectiveFovLH(m_Fov, static_cast<float>(m_Width), static_cast<float>(m_Height), m_Near, m_Far);
	//m_ProjectionMatrix[1].y *= -1;
	/*m_ProjectionMatrix = { {1 / (float(m_width) / m_Height * m_Fov),0,0,0},
							{0, 1 / m_Fov, 0, 0},
							{0, 0, m_Far / (m_Far - m_Near), 1},
							{0, 0, -(m_Far * m_Near) / (m_Far - m_Near), 0} };*/
}

void Camera::Update(/*const Timer* pTimer*/)
{
	//const float deltaTime = pTimer->GetElapsed();
	////Keyboard Input
	//const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	////Mouse Input
	//int mouseX{}, mouseY{};
	//const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

	//HandleKeyMovement(pKeyboardState, deltaTime);

	//if (mouseState == SDL_BUTTON(SDL_BUTTON_RIGHT))
	//{
	//	if (mouseX != 0 || mouseY != 0)
	//	{
	//		m_TotalPitch -= mouseY;
	//		if (m_TotalPitch >= 89) m_TotalPitch = 89;
	//		if (m_TotalPitch <= -89) m_TotalPitch = -89;
	//		m_TotalYaw += mouseX;
	//		TransformForwardVector();
	//	}
	//}

	//if (mouseState == SDL_BUTTON(SDL_BUTTON_LEFT))
	//{
	//	if (mouseY != 0) m_Origin += m_Forward * float(-mouseY) * m_TranslateSpeed * deltaTime * 10.f;

	//	if (mouseX != 0)
	//	{
	//		m_TotalYaw += mouseX;
	//		TransformForwardVector();
	//	}
	//}
	//if (mouseState == (SDL_BUTTON(SDL_BUTTON_LEFT) | SDL_BUTTON(SDL_BUTTON_RIGHT)))
	//{
	//	if (mouseY != 0) m_Origin.y += float(mouseY) * m_TranslateSpeed * deltaTime * 10.f;
	//}

}
void Camera::KeyEvent(int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Origin += m_Forward * 0.5f;
	}
	if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Origin -= m_Forward * 0.5f;
	}
	if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Origin -= m_Right * 0.5f;
	}
	if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Origin += m_Right * 0.5f;
	}
	if (key == GLFW_KEY_E && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Origin += m_Up * 0.5f;
	}
	if (key == GLFW_KEY_Q && (action == GLFW_REPEAT || action == GLFW_PRESS))
	{
		m_Origin -= m_Up * 0.5f;
	}
	TransformForwardVector();
}
void Camera::MouseMove(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		
		float dx = static_cast<float>(xpos) - m_DragStart.x;
		if (dx > 0) 
		{
			m_TotalYaw += 0.01f;
			m_DragStart.x = xpos;
		}
		
		else if (dx < 0)
		{ 
			m_TotalYaw -= 0.01f;
			m_DragStart.x = xpos;
		}

		float dy = static_cast<float>(ypos) - m_DragStart.y;
		if (dy > 0) 
		{
			m_TotalPitch -= 0.01f;
			m_DragStart.y = ypos;
		}
		else if (dy < 0)
		{
			m_TotalPitch += 0.01f;
			m_DragStart.y = ypos;
		}
		
		TransformForwardVector();
	}
}
void Camera::MouseEvent(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		std::cout << "right mouse button pressed\n";
		double xpos, ypos;
		
		glfwGetCursorPos(window, &xpos, &ypos);
		//m_DragStart.x = static_cast<float>(xpos);
		//m_DragStart.y = static_cast<float>(ypos);
	}
}
void Camera::HandleKeyMovement(const uint8_t* keys, float elapsedSec)
{
	/*float speed{ (keys[SDL_SCANCODE_LSHIFT] ? m_TranslateSpeed * 3 : m_TranslateSpeed) };

	if (keys[SDL_SCANCODE_Z])
	{
		m_FovAngle += m_TranslateSpeed * elapsedSec;
		if (m_FovAngle > 360) m_FovAngle = 360;
		m_Fov = tan(TO_RADIANS * m_FovAngle / 2);
		CalculateProjectionMatrix();
		std::cout << m_FovAngle << std::endl;
	}
	if (keys[SDL_SCANCODE_C])
	{
		m_FovAngle -= m_TranslateSpeed * elapsedSec;
		if (m_FovAngle < 0) m_FovAngle = 0;
		m_Fov = tan(TO_RADIANS * m_FovAngle / 2);
		CalculateProjectionMatrix();
		std::cout << m_FovAngle << std::endl;
	}*/

	//Update Matrices
	CalculateViewMatrix();
}
void Camera::TransformForwardVector()
{
	glm::mat4x4 finalRotation = glm::rotate(glm::mat4x4(1.f), m_TotalYaw , glm::vec3{0,1,0 });
	finalRotation = glm::rotate(finalRotation, m_TotalPitch , glm::vec3{1,0,0});
		
	m_Forward = glm::normalize(finalRotation[2]);
	//m_Forward = finalRotation.TransformVector(glm::vec3::UnitZ).Normalized();
	CalculateViewMatrix();
}

