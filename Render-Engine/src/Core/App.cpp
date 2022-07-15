#include "pch.h"
#include "App.h"
#include "../Core.h"
#include "../Window/Window.h"

namespace rnd {

	void App::Run() {
		while (!m_Window.ShouldClose()) {
			glfwPollEvents();
		}
	}

}