#pragma once

#include "../Core.h"
#include "../Defs.h"

namespace rnd {

	class Window final {
	public:
		RENDER_API Window(int width, int height, std::string title);
		RENDER_API ~Window();

		RENDER_API bool ShouldClose();

	private:
		RENDER_API void CreateWindow();

	private:
		const int m_Width;
		const int m_Height;
		std::string m_Title;

		GLFWwindow* m_Window;
		uint32_t m_ExtensionCount = 0;

	};

}

