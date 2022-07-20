#pragma once

#include "../Defs.h"
#include "../Window/Window.h"

namespace rnd {
	class App final {
	public:
		RENDER_API void Run();

	private:
		void Init();
		void MainLoop();
		void CleanUp();
		void CreateInstance();

		Window m_Window { 800, 600, "Vulkan" };
		VkInstance m_VkInstance;
	};
}