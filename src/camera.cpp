#include "camera.hpp"

static bool is_first_mouse{ false };
static bool is_rmb_down{ false };
static double last_x{ 0.0f };
static double last_y{ 0.0f };
static glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };

void Camera::Move(int dir, float deltatime)
{
	float speed = m_MovementSpeed * deltatime;
	auto posv4 = glm::vec4(m_Origin, 1.0f);
	if (dir == FORWARD)
		m_Origin = glm::translate(m_Forward * speed) * posv4;
	if (dir == BACKWARD)
		m_Origin = glm::translate(-m_Forward * speed) * posv4;
	if (dir == RIGHT)
		m_Origin = glm::translate(m_Right * speed) * posv4;
	if (dir == LEFT)
		m_Origin = glm::translate(-m_Right * speed) * posv4;
	if (dir == UP)
		m_Origin = glm::translate(WORLD_UP * speed) * posv4;
	if (dir == DOWN)
		m_Origin = glm::translate(-WORLD_UP * speed) * posv4;
}

void Camera::MouseCallback(double xpos, double ypos)
{
	if (is_first_mouse) {
		last_x = xpos;
		last_y = ypos;
		is_first_mouse = false;
	}

	if (is_rmb_down) {
		float delta_x = static_cast<float>(xpos - last_x);
		float delta_y = static_cast<float>(last_y - ypos);
		last_x = xpos;
		last_y = ypos;

		static float sensitivity = 0.05f;
		delta_x *= sensitivity;
		delta_y *= sensitivity;

		glm::mat4 yaw = glm::rotate(m_RotationSpeed * -delta_x, WORLD_UP);
		glm::mat4 pitch = glm::rotate(m_RotationSpeed * delta_y, glm::normalize(glm::cross(m_Forward, WORLD_UP)));

		m_Forward = glm::vec3(pitch * yaw * glm::vec4(m_Forward, 0.0f));

		m_Right = glm::normalize(glm::cross(m_Forward, WORLD_UP));
		m_Up = glm::normalize(glm::cross(m_Right, m_Forward));
	}
}

void Camera::MouseButtonCallback(double cursor_x, double cursor_y, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		is_rmb_down = true;
		last_x = cursor_x;
		last_y = cursor_y;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
		is_rmb_down = false;
}