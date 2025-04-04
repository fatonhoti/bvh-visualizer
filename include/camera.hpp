#pragma once

#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include <GLFW/glfw3.h>

constexpr int FORWARD{ 0 };
constexpr int BACKWARD{ 1 };
constexpr int RIGHT{ 2 };
constexpr int LEFT{ 3 };
constexpr int UP{ 4 };
constexpr int DOWN{ 5 };

class Camera
{
public:
	Camera() = default;

	inline glm::vec3 GetForward() const { return m_Forward; }
	inline glm::vec3 GetUp() const { return m_Up; }
	inline glm::vec3 GetRight() const { return m_Right; }
	inline glm::vec3 GetOrigin() const { return m_Origin; }
	inline glm::mat4 GetViewMatrix() const { return glm::lookAt(m_Origin, m_Origin + m_Forward, m_Up); }

	inline void SetOrigin(glm::vec3 origin) { m_Origin = origin; }

	inline glm::mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
	void UpdateProjectionMatrix() {
		m_ProjectionMatrix = glm::perspective(glm::radians(m_VerticalFOV), m_AspectRatio, m_NearPlane, m_FarPlane);
	}

	inline float GetVerticalFOV() const { return m_VerticalFOV; }
	inline float GetAspectRatio() const { return m_AspectRatio; }
	inline float GetNearPlane() const { return m_NearPlane; }
	inline float GetFarPlane() const { return m_FarPlane; }

	inline void SetVerticalFOV(float fovy) { m_VerticalFOV = fovy; UpdateProjectionMatrix(); }
	inline void SetAspectRatio(float ar) { m_AspectRatio = ar; UpdateProjectionMatrix(); }
	inline void SetNearPlane(float near) { m_NearPlane = near; UpdateProjectionMatrix(); }
	inline void SetFarPlane(float far) { m_FarPlane = far; UpdateProjectionMatrix(); }

	inline float GetMovementSpeed() const { return m_MovementSpeed; }
	inline float GetRotationSpeed() const { return m_RotationSpeed; }

	void Move(int dir, float deltatime);
	void MouseCallback(double xpos, double ypos);
	void MouseButtonCallback(double cursor_x, double cursor_y, int button, int action, int mods);

	float m_MovementSpeed{ 15.0f };
private:
	// Basis vectors
	glm::vec3 m_Forward{ 0.0f, 0.0f, -1.0f };
	glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };
	glm::vec3 m_Right{ 1.0f, 0.0f, 0.0f };
	glm::vec3 m_Origin{ 0.0f, 0.0f, 0.0f };

	// Projection matrix properties
	glm::mat4 m_ProjectionMatrix{ 1.0f };
	float m_VerticalFOV{ 45.0f };
	float m_AspectRatio{ 1.77777778f }; // 1280x720
	float m_NearPlane{ 0.01f };
	float m_FarPlane{ 300.0f };

	// Movement properties
	float m_RotationSpeed{ 0.10f };
};