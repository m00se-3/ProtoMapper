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
module;

#include <filesystem>
#include <memory>
#include <string>
#include <print>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

export module proto.Window;

import proto.UI.Container;

namespace proto
{

    export class Window
    {
    public:

        Window();
        ~Window();

        [[nodiscard]]GLFWwindow* GetPtr() const;
        [[nodiscard]]bool Construct(const std::string& title, bool fullscreen);
        [[nodiscard]]int GetWidth() const;
		[[nodiscard]]int GetHeight() const;

        void SetSize(int w, int h);

        [[nodiscard]]static UIContainer* UI();

        static void SetUIHandle(UIContainer* ptr);

        
        static void DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

        /*
			GLFW callback functions
		*/

		static void ContextErrorMessage(int code, const char* description);
		static void MonitorCallback(GLFWmonitor* monitor, int event);
		static void WindowCloseCallback(GLFWwindow* window);
		static void WindowMaximizeCallback(GLFWwindow* window, int maximized);
		static void WindowMinimizedCallback(GLFWwindow* window, int iconified);


    private:

        GLFWwindow* _window = nullptr;
		GLFWmonitor* _monitor = nullptr;    // There is no terminate function for GLFWmonitors.

        int _wWidth = 1024, _wHeight = 768;

        static UIContainer* _ui;

    };

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

		//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		

		_monitor = glfwGetPrimaryMonitor();

		if (fullscreen)
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, title.c_str(), _monitor, nullptr);
		}
		else
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, title.c_str(), nullptr, nullptr);
			//glfwSetWindowPos(_window, 100, 100);
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
		std::println("Error {} {} {} - {}", src, type, severity, message);
	}

	void Window::ContextErrorMessage(int code, const char* description)
	{
		std::println("Error code {} - {}", code, description);
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