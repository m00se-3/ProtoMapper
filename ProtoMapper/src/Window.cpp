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
#include <GLFW/glfw3.h>

namespace proto
{

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
			glfwSetWindowPos(_window, 100, 100);
		}
		else
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, title.c_str(), nullptr, nullptr);
			glfwSetWindowPos(_window, 100, 100);
		}

		if (_window != nullptr)
		{
			auto getProcAddress = [](const char* proc) -> void*{
				return (void*)glfwGetProcAddress(proc);
			};
			
			glfwMakeContextCurrent(_window);
			glfwSwapInterval(1);

			if (gladLoadGLLoader(getProcAddress) == 0)
			{
				return false;
			}

			glDebugMessageCallback((GLDEBUGPROC)Window::DebugOpenGL, nullptr);

			// Set GLFW event callbacks.

			glfwSetWindowMaximizeCallback(_window, Window::WindowMaximizeCallback);
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

	void Window::RefreshSize() { glfwGetWindowSize(_window, &_wWidth, &_wHeight); }

    void Window::DebugOpenGL(GLenum src, GLenum type, [[maybe_unused]] GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam)
	{
		std::fprintf(stderr, "Error %d %d %d - %s", src, type, severity, message); //NOLINT
	}

	void Window::ContextErrorMessage(int code, const char* description)
	{
		std::fprintf(stderr, "Error code %d - %s", code, description); // NOLINT
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

		self->SetContextSize(width, height);
		self->GetRenderer()->RefreshProjection();

		const int rX = self->GetRenderer()->GetRenderX(),
			rY = self->GetRenderer()->GetRenderY();


		self->GetRenderer()->SetViewport(rX, rY, width, height);

	}

	void Window::WindowMaximizeCallback([[maybe_unused]] GLFWwindow* window, int maximized)
	{
		Mapper::GetInstance()->SetFullscreen((bool)maximized);
	}

}
