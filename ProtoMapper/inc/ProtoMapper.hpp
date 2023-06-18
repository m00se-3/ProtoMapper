/*
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

#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "SimpleIni.h"
#include "Scene.hpp"

#include <string>
#include <memory>
#include <filesystem>

// Forward declarations.
class Renderer;
class ResourceManager;

class ProtoMapper
{
	const std::string _title = "ProtoMapper - ";
	std::filesystem::path _rootDir;
	bool _appRunning = true, _mapOpen = true, _panning = false, _fullscreen = true, _configUpdate = false;
	
	float _fWidth = 1024.f, _fHeight = 768.f;
	int _wWidth = 1024, _wHeight = 768;

	GLFWwindow* _window = nullptr;
	GLFWmonitor* _monitor = nullptr;

	std::unique_ptr <Scene> _scene;
	std::unique_ptr<Renderer> _renderer;
	std::unique_ptr<ResourceManager> _resources;

	char* _textMemoryBuffer = nullptr;

	std::filesystem::path _configFile;
	CSimpleIniA _configData;

	// Self pointer for use in the GLFW callbacks.
	static ProtoMapper* _self;

public:
	ProtoMapper() = default;
	~ProtoMapper();

	bool Configure();
	void Run();

	static void DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	/*
		GLFW callback functions
	*/

	static void ContextErrorMessage(int code, const char* description);
	static void MonitorCallback(GLFWmonitor* monitor, int event);
	static void KeyboardEventCallback(GLFWwindow* window, int keyn, int scancode, int action, int mods);
	static void TextEventCallback(GLFWwindow* window, unsigned int codepoint);
	static void MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollEventCallback(GLFWwindow* window, double offX, double offY);
	static void DropEventCallback(GLFWwindow* window, int count, const char** paths);

};

#endif