#include "pch.h"

#include "../Defs.h"
#include "../Core.h"
#include "Window.h"
#include <string>

namespace Render {

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
		
		//glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_PWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_PWindow, this);
		glfwSetFramebufferSizeCallback(m_PWindow, FramebufferResizeCallback);
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto tWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		tWindow->framebufferResized = true;
		tWindow->_SetWidth(width);
		tWindow->_SetHeight(height);
	}

	bool Window::ShouldClose() {
		return glfwWindowShouldClose(m_PWindow);
	}

	GLFWwindow* Window::GetGLFWwindowPointer() {
		return m_PWindow;
	}

	int Window::GetWidth() {
		return m_Width;
	}

	int Window::GetHeight() {
		return m_Height;
	}

	void Window::_SetWidth(int width) {
		m_Width = width;
	}

	void Window::_SetHeight(int height) {
		m_Height = height;
	}

}
