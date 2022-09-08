#pragma once

#include "../Defs.h"

struct GLFWWindow;

namespace Render {

	class Window final {
	public:
		RENDER_API Window(int width, int height, std::string title);
		RENDER_API ~Window();

		RENDER_API bool ShouldClose();

		GLFWwindow* GetGLFWwindowPointer();

	private:
		RENDER_API void CreateWindow();

	private_var:
		const int m_Width;
		const int m_Height;
		std::string m_Title;

		GLFWwindow* m_PWindow;
		uint32_t m_ExtensionCount = 0;

	};

}

