#include "pch.h"
#include "Window.h"
#include <string>

namespace rnd {

	Window::Window(int width, int height, std::string title)
		: m_Width(width), m_Height(height), m_Title(title) {
		CreateWindow();
	}

	Window::~Window() {
		glfwDestroyWindow(m_PWindow);
		glfwTerminate();
	}

	void Window::CreateWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_PWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
	}

	bool Window::ShouldClose() {
		return glfwWindowShouldClose(m_PWindow);
	}

	GLFWwindow* Window::GetGLFWwindowPointer() {
		return m_PWindow;
	}


}
