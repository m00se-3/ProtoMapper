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
#ifndef PROTO_WINDOW_HPP
#define PROTO_WINDOW_HPP

#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace proto
{
    class Window
    {
    public:

        [[nodiscard]]GLFWwindow* GetPtr() const;
        [[nodiscard]]bool Construct(const std::string& title, bool fullscreen);
        [[nodiscard]]int GetWidth() const;
	    [[nodiscard]]int GetHeight() const;

        void SetSize(int w, int h);
        void RefreshSize();

        static void DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

        /*
                GLFW callback functions. Only the ones I intend to use are included, there are more available.
        */

        static void ContextErrorMessage(int code, const char* description);
        static void WindowMaximizeCallback(GLFWwindow* window, int maximized);
        static void DropEventCallback(GLFWwindow* window, int count, const char** paths);
        static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);


    private:

        GLFWwindow* _window = nullptr;
    	GLFWmonitor* _monitor = nullptr;    // There is no terminate function for GLFWmonitors.

        int _wWidth = 1024, _wHeight = 768;

    };
}

#endif
