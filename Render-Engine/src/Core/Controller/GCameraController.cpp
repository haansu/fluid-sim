#include "pch.h"
#include "GCameraController.h"

// Core
#include <Core.h>
#include <Core/Display/GCamera.h>
#include <Rnd/Time.h>
#include <Window/Window.h>
#include <Core/UI/UI.h>
//

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
//

namespace Render {

	GCameraController::GCameraController(Window* window, GCamera* camera) {
		m_PWindow = window;
		m_PGCamera = camera;
	}

	void GCameraController::Update() {
		glm::vec3 velocity{ 0.0f };

		float velSpeed = 50.0f * rnd::Time::DeltaTime();
		float rotSpeed = 0.5f * rnd::Time::DeltaTime();

		double cPosX, cPosY;
		glfwGetCursorPos(m_PWindow->GetGLFWwindowPointer(), &cPosX, &cPosY);

		m_CursorPos.x = static_cast<int>(cPosX);
		m_CursorPos.y = static_cast<int>(cPosY);

		// Position
		int keyStateW = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_W);
		int keyStateA = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_A);
		int keyStateS = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_S);
		int keyStateD = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_D);

		int keyStateLShift = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_LEFT_SHIFT);

		// Rotation
		int keyStateLeft = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_LEFT);
		int keyStateRight = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_RIGHT);
		int keyStateUp = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_UP);
		int keyStateDown = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_DOWN);


		// Use cursor
		int keyStateSpace = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_SPACE);

		if (keyStateLShift == GLFW_PRESS)
			velSpeed = 100.0f * rnd::Time::DeltaTime();

		glm::ivec2 cursorDelta{ 0 };
		if (m_CursorPos.x < m_PWindow->GetWidth()
			&& m_CursorPos.x > 0
			&& m_CursorPos.y < m_PWindow->GetHeight()
			&& m_CursorPos.y > 0
			&& keyStateSpace == GLFW_PRESS) {

			cursorDelta = m_PrevCursorPos - m_CursorPos;
		}

		glm::mat4 rotMat{ 1.0f };
		glm::vec3 pitchAxis = glm::vec3(glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3{ 0.0f, 0.0f, 1.0f }) * glm::vec4(glm::normalize(glm::vec3(m_Rotation[0], m_Rotation[1], 0.0f)), 1.0f));

		// Cursor rotation based on delta
		if (cursorDelta.y != 0)
			rotMat = glm::rotate(rotMat, -static_cast<float>(cursorDelta.y) / 1500.0f, pitchAxis);

		if (cursorDelta.x != 0)
			rotMat = glm::rotate(rotMat, static_cast<float>(cursorDelta.x) / 1500.0f, glm::vec3{0.0f, 0.0f, 1.0f});

		// Rotation based on key input
		if (keyStateUp == GLFW_PRESS)
			rotMat = glm::rotate(rotMat, -rotSpeed, pitchAxis);
		if (keyStateLeft == GLFW_PRESS)
			rotMat = glm::rotate(rotMat, rotSpeed, glm::vec3{ 0.0f, 0.0f, 1.0f });
		if (keyStateDown == GLFW_PRESS)
			rotMat = glm::rotate(rotMat, rotSpeed, pitchAxis);
		if (keyStateRight == GLFW_PRESS)
			rotMat = glm::rotate(rotMat, -rotSpeed, glm::vec3{ 0.0f, 0.0f, 1.0f });

		m_Forward = glm::vec3(rotMat * glm::vec4(m_Forward, 1.0f));
		glm::vec3 right{ m_Forward.y, -m_Forward.x, 0.0f };

		m_Rotation = glm::vec3(rotMat * glm::vec4(m_Rotation, 1.0f));
		

		if (keyStateW == GLFW_PRESS)
			velocity += m_Forward;
		if (keyStateA == GLFW_PRESS)
			velocity += -right;
		if (keyStateS == GLFW_PRESS)
			velocity += -m_Forward;
		if (keyStateD == GLFW_PRESS)
			velocity += right;

		if (velocity != glm::vec3{0.0f, 0.0f, 0.0f})
			velocity = glm::normalize(velocity) * velSpeed;		

		m_Position += velocity;

		// Set data to show on screen. Could call a function with parameters.
		UI::cameraPosition[0] = m_Position[0];
		UI::cameraPosition[1] = m_Position[1];
		UI::cameraPosition[2] = m_Position[2];
		UI::cameraRotation[0] = m_Rotation[0];
		UI::cameraRotation[1] = m_Rotation[1];
		UI::cameraRotation[2] = m_Rotation[2];
		UI::cameraVelocity[0] = velocity[0];
		UI::cameraVelocity[1] = velocity[1];
		UI::cameraVelocity[2] = velocity[2];

		UI::cursorDelta[0] = cursorDelta[0];
		UI::cursorDelta[1] = cursorDelta[1];

		UpdateTransform();
		m_PrevCursorPos = m_CursorPos;
	}

	void GCameraController::UpdateTransform() {
		m_PGCamera->SetViewDir(m_Position, m_Rotation);

	}

}
