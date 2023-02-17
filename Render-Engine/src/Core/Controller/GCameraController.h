#pragma once

#include "GController.h"
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace Render {

	class GCamera;
	class Window;

	class GCameraController : public GController {
	public:
		GCameraController(Window* window, GCamera* camera);
		
		void Update();

	private:
		void SetPos();
		void SetRot();

		Window* m_PWindow = nullptr;
		GCamera* m_PGCamera = nullptr;

		glm::vec3 m_CurrentPos{1.0f};
		glm::ivec2 m_CursorPos{ 0 };
	};

}
