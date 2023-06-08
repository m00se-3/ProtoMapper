#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "SimpleIni.h"
#include "Scene.hpp"

#include <string>
#include <memory>

// Forward declarations.
class Renderer;
class ResourceManager;

class ProtoMapper
{
	const std::string _title = "ProtoMapper - ";
	std::string _assetsDir;
	bool _appRunning = true, _mapOpen = true, _panning = false, _fullscreen = true, _configUpdate = false;
	
	Scene _scene;

	float _fWidth = 1024.f, _fHeight = 768.f;
	int _wWidth = 1024, _wHeight = 768;

	GLFWwindow* _window = nullptr;
	GLFWmonitor* _monitor = nullptr;
	std::unique_ptr<Renderer> _renderer;
	std::unique_ptr<ResourceManager> _resources;

	std::string _configFile;
	CSimpleIniA _configData;

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
};

#endif