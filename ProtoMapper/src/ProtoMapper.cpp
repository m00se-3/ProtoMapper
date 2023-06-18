﻿/*
	ProtoMapper - Map creation and pathfinding software for game development.
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

#include "ProtoMapper.hpp"

#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>

ProtoMapper* ProtoMapper::_self = nullptr;

constexpr const size_t InitialTextBufferSize = 4u * 1024u;	// Allocate 4 KB for the text memory buffer. Can change later if needed.


void ProtoMapper::DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]GLsizei length, const GLchar* message, [[maybe_unused]]const void* userParam)
{
	printf_s("Error [%u] [%u] [%u] - %s", src, type, severity, message);
}

void ProtoMapper::ContextErrorMessage(int code, const char* description)
{
	printf_s("Error code [%u] - %s\n", code, description);
}

void ProtoMapper::MonitorCallback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED)
	{

	}
	else if(event == GLFW_DISCONNECTED)
	{

	}
}

void ProtoMapper::KeyboardEventCallback(GLFWwindow* window, int keyn, int scancode, int action, int mods)
{
}

void ProtoMapper::TextEventCallback(GLFWwindow* window, unsigned int codepoint)
{
}

void ProtoMapper::MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods)
{
}

void ProtoMapper::MouseScrollEventCallback(GLFWwindow* window, double offX, double offY)
{
}

void ProtoMapper::DropEventCallback(GLFWwindow* window, int count, const char** paths)
{

}

ProtoMapper::~ProtoMapper() 
{
	if (_configUpdate)
	{
		_configData.SaveFile(_configFile.c_str());
	}

	_configData.Reset();
	_scene->Cleanup();
	_scene.reset(nullptr);
	_renderer.reset(nullptr);
	_resources.reset(nullptr);

	delete[] _textMemoryBuffer;

	glfwDestroyWindow(_window);

	glfwTerminate();
}

bool ProtoMapper::Configure()
{
	_self = this;
	
	glfwSetErrorCallback(ProtoMapper::ContextErrorMessage);

	if (!glfwInit())
	{
		return false;
	}

	/*
		Images are loaded the right way up.
	*/
	stbi_set_flip_vertically_on_load(1);


#if defined(_DEBUG_) || defined(_RELEASE_)

	_rootDir = ROOT_DIR;
	_configFile = _rootDir.string() + "/config/config.ini";

#else
	_rootDir = "."
	_configFile = _rootDir.string() + "/config/config.ini"

#endif // _DEBUG_

		if (std::filesystem::exists(_configFile))
		{
			auto err = _configData.LoadFile(_configFile.string().c_str());

			if (err < 0) return false;
		}
		else
			return false;


	auto width = _configData.GetLongValue("startup_display", "width", -1);
	auto height = _configData.GetLongValue("startup_display", "height", -1);

	if (width < 0 || height < 0)
		return false;

	/*
		If one of the dimensions is zero, it's safe to assume the native display is being used.
	*/
	if (width != 0 && height != 0)
	{
		_wWidth = static_cast<unsigned int>(width);
		_wHeight = static_cast<unsigned int>(height);
		_fWidth = static_cast<float>(width);
		_fHeight = static_cast<float>(height);
		_fullscreen = false;
	}

	/*
		Set up the ResourceManager object and hook it up to the resource types it needs to track.
	*/

	_textMemoryBuffer = new char[InitialTextBufferSize];

	_resources = std::make_unique<ResourceManager>(_textMemoryBuffer, InitialTextBufferSize);

	Renderer::SetResourceManager(_resources.get());
	Texture2D::SetResourceManager(_resources.get());
	Shader::SetResourceManager(_resources.get());
	Scene::SetResourceManager(_resources.get());

	return true;
}


void ProtoMapper::Run()
{
	using time = std::chrono::high_resolution_clock;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_monitor = glfwGetPrimaryMonitor();
	
	if (_fullscreen)
	{
		_window = glfwCreateWindow(_wWidth, _wHeight, std::string{_title + VERSION_NUMBER}.c_str(), _monitor, nullptr);
	}
	else
	{
		_window = glfwCreateWindow(_wWidth, _wHeight, std::string{_title + VERSION_NUMBER}.c_str(), nullptr, nullptr);
	}

	if (_window)
	{
		glfwMakeContextCurrent(_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

		glDebugMessageCallback((GLDEBUGPROC)ProtoMapper::DebugOpenGL, nullptr);

		glfwSetMonitorCallback(ProtoMapper::MonitorCallback);
		glfwSetKeyCallback(_window, ProtoMapper::KeyboardEventCallback);
		glfwSetCharCallback(_window, ProtoMapper::TextEventCallback);
		glfwSetMouseButtonCallback(_window, ProtoMapper::MouseButtonEventCallback);
		glfwSetScrollCallback(_window, ProtoMapper::MouseScrollEventCallback);
		glfwSetDropCallback(_window, ProtoMapper::DropEventCallback);
	}
	else
	{
		return;
	}

	_renderer = std::make_unique<Renderer>(_rootDir.string().c_str());

	_renderer->SetRenderWindow(_fWidth, _fHeight);
	_renderer->Init(Renderer::mode::Two);


	glClearColor(0.f, 0.f, 0.f, 1.f);

	time::time_point last = time::now();

	_scene = std::make_unique<Scene>();
	_scene->Init();

	glfwShowWindow(_window);

	while (!glfwWindowShouldClose(_window) && _appRunning)
	{
		time::time_point current = time::now();
		float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_scene->Update(microseconds * 1000000.f);

		_scene->DrawNodes();
		_scene->CompileUI();
		_scene->DrawUI(_renderer.get());
		
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}

}


int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{
	ProtoMapper mapper;

	if (mapper.Configure())
	{
		mapper.Run();
	}
	else
	{
		fprintf_s(stdout, "Failed to load application configurations. Please reinstall the program.");
	}

	return 0; // Don't forget to return 0 from an SDL application. :$
}