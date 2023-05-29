#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "SDL2/SDL.h"
#include "glad/glad.h"
#include "SimpleIni.h"
#include "Scene.hpp"

#include <string>
#include <memory>

// Forward declarations.
class Renderer;

class ProtoMapper
{
	const std::string _title = "ProtoMapper - ";
	std::string _assetsDir;
	bool _appRunning = true, _mapOpen = true, _panning = false, _fullscreen = true, _configUpdate = false;
	
	Scene _scene;

	float _fWidth = 1024.f, _fHeight = 768.f;
	int _wWidth = 1024, _wHeight = 768;

	SDL_Window* _window = nullptr;
	SDL_GLContext _mapContext;
	std::unique_ptr<Renderer> _renderer;

	std::string _configFile;
	CSimpleIniA _configData;

public:
	ProtoMapper() = default;
	~ProtoMapper();

	void CreateToolBar();

	bool Configure();
	void Run();

	static void DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
};

#endif