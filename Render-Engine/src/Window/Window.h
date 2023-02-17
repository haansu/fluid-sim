#pragma once

#include "../Defs.h"
#include "../Core.h"

namespace Render {

	class Window final {
	public:
		RENDER_API Window(int width, int height, std::string title);
		RENDER_API ~Window();

		RENDER_API bool ShouldClose();

		GLFWwindow* GetGLFWwindowPointer();

		int GetWidth();
		int GetHeight();

	private:
		RENDER_API void CreateWindow();
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

		void _SetWidth(int width);
		void _SetHeight(int height);

	public_var:
		bool framebufferResized = false;

	private_var:
		int m_Width;
		int m_Height;
		std::string m_Title;


		GLFWwindow* m_PWindow;
		uint32_t m_ExtensionCount = 0;
		
		friend class App;
	};

}
