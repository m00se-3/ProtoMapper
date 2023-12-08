/*
	ProtoWindow - Map creation and pathfinding software for game development.
	Copyright (C) 2023  Samuel Bridgham - moosethree473@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "Window.hpp"

#include "ProtoMapper.hpp"

namespace proto
{
	UIContainer* Window::_ui = nullptr;

    UIContainer* Window::UI() { return _ui; }

    void Window::SetUIHandle(UIContainer* ptr) { _ui = ptr; }

    Window::Window()
    {
        glfwSetErrorCallback(Window::ContextErrorMessage);
    }

    Window::~Window()
    {
        glfwDestroyWindow(_window);
		glfwTerminate();
    }

    bool Window::Construct(const std::string& title, bool fullscreen)
    {
        /*
			Create the window and the opengl context.
		*/

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		

		_monitor = glfwGetPrimaryMonitor();

		if (fullscreen)
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, title.c_str(), _monitor, nullptr);
		}
		else
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, title.c_str(), nullptr, nullptr);
			glfwSetWindowPos(_window, 100, 100);
		}

		if (_window)
		{
			glfwMakeContextCurrent(_window);
			glfwSwapInterval(1);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;

			glDebugMessageCallback((GLDEBUGPROC)Window::DebugOpenGL, nullptr);

			// Set GLFW event callbacks.

			glfwSetMonitorCallback(Window::MonitorCallback);
			glfwSetWindowCloseCallback(_window, Window::WindowCloseCallback);
			glfwSetWindowMaximizeCallback(_window, Window::WindowMaximizeCallback);
			glfwSetWindowIconifyCallback(_window, Window::WindowMinimizedCallback);
			glfwSetDropCallback(_window, Window::DropEventCallback);
			glfwSetFramebufferSizeCallback(_window, Window::FrameBufferSizeCallback);

		}
		else
		{
			return false;
		}

        return true;
    }

    GLFWwindow* Window::GetPtr() const { return _window; }

    int Window::GetWidth() const { return _wWidth; }
	
	int Window::GetHeight() const { return _wHeight; }

    void Window::SetSize(int w, int h)
    {
        _wWidth = w;
        _wHeight = h;
    }

    void Window::DebugOpenGL(GLenum src, GLenum type, [[maybe_unused]] GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam)
	{
		std::puts(std::format("Error {} {} {} - {}", src, type, severity, message).c_str());
	}

	void Window::ContextErrorMessage(int code, const char* description)
	{
		std::puts(std::format("Error code {} - {}", code, description).c_str());
	}

	void Window::DropEventCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]]int count, [[maybe_unused]] const char** paths)
	{
	}

	void Window::FrameBufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height)
	{
		// If the window was minimized, do nothing.
		if(width == 0 || height == 0)
		{
			return;
		}
		
		auto* self = Mapper::GetInstance();

		int oW = self->GetWin().GetWidth(),
			oH = self->GetWin().GetHeight(),
			rX = self->GetRenderer()->GetRenderX(),
			rY = self->GetRenderer()->GetRenderY(),
			rW = self->GetRenderer()->GetRenderWidth(),
			rH = self->GetRenderer()->GetRenderHeight();

		float sW = ((float)width / (float)oW);
		float sH = ((float)height / (float)oH);

		self->SetContextSize(width, height);
		self->GetRenderer()->RefreshProjection();

		const int nx = std::lround((float)rX * sW);
		const int ny = std::lround((float)rY * sH);
		const int nw = std::lround((float)rW * sW);
		const int nh = std::lround((float)rH * sH);

		self->GetRenderer()->SetViewport(nx, ny, nw, nh);

	}

	void Window::MonitorCallback([[maybe_unused]] GLFWmonitor* monitor, [[maybe_unused]] int event)
	{
		if (event == GLFW_CONNECTED)
		{

		}
		else if (event == GLFW_DISCONNECTED)
		{

		}
	}

	void Window::WindowCloseCallback(GLFWwindow* window)
	{
		
	}

	void Window::WindowMaximizeCallback(GLFWwindow* window, int maximized)
	{
		
	}

	void Window::WindowMinimizedCallback(GLFWwindow* window, int iconified)
	{

	}
}