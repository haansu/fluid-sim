#include "pch.h"
#include "GCameraController.h"

#include "../../Core.h"
#include "../../Window/Window.h"
#include "../Display/GCamera.h"
#include <Core/Time.h>

namespace Render {
	GCameraController::GCameraController(Window* window, GCamera* camera) {
		m_PWindow = window;
		m_PGCamera = camera;
	}

	void GCameraController::Update() {
		glm::vec3 velocity{ 0.0f };
		double cPosX, cPosY;
		glfwGetCursorPos(m_PWindow->GetGLFWwindowPointer(), &cPosX, &cPosY);
		m_CursorPos.x = static_cast<int>(cPosX);
		m_CursorPos.y = static_cast<int>(cPosY);

		

		int keyStateW = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_W);
		int keyStateA = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_A);
		int keyStateS = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_S);
		int keyStateD = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_D);

		int keyStateLShift = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_KEY_LEFT_SHIFT);
		int keyStateMouseLeft = glfwGetKey(m_PWindow->GetGLFWwindowPointer(), GLFW_MOUSE_BUTTON_LEFT);

		float val = 1.0f * Time::DeltaTime();
		if (keyStateLShift == GLFW_PRESS)
			val = 2.0f * Time::DeltaTime();

		if (keyStateW == GLFW_PRESS)
			velocity.x = val;
		if (keyStateA == GLFW_PRESS)
			velocity.y = -val;
		if (keyStateS == GLFW_PRESS)
			velocity.x = -val;
		if (keyStateD == GLFW_PRESS)
			velocity.y = val;

		
		if (   m_CursorPos.x < m_PWindow->GetWidth()
			&& m_CursorPos.x > 0
			&& m_CursorPos.y <  m_PWindow->GetHeight()
			&& m_CursorPos.y > 0) {
			std::cout << m_CursorPos[0] << " " << m_CursorPos[1] << "\n";
		}

		glm::vec3 currentPos = m_PGCamera->GetPosVec();
		currentPos = currentPos + velocity;
		m_PGCamera->SetViewDir(currentPos, glm::vec3{ 1.0f, 0.0f, 0.0f });
	}

	void GCameraController::SetPos() {

	}

	void GCameraController::SetRot() {
	
	}

}
