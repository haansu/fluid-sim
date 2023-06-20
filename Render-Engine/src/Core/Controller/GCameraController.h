#pragma once

// Core
#include "GController.h"
//

// GLM
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
//

namespace Render {

	class GCamera;
	class Window;

	class GCameraController : public GController {
	public:
		GCameraController(Window* window, GCamera* camera);
		
		void Update();

	private:
		void UpdateTransform();

		Window* m_PWindow = nullptr;
		GCamera* m_PGCamera = nullptr;

		glm::vec3 m_Position{ 10.0f, -20.0f, 10.0f };
		glm::vec3 m_Rotation{ 0.0f, 1.0f, 0.0f };
		glm::vec3 m_PitchAxis{ 1.0f, 0.0f, 0.0f };

		glm::vec3 m_Forward{ 0.0f, 1.0f, 0.0f };

		glm::ivec2 m_CursorPos{ 0 };
		glm::ivec2 m_PrevCursorPos{ 0 };
	};

}
