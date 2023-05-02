#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "SDL2/SDL.h"
#include "TGUI/Core.hpp"
#include "TGUI/Backends/SDL.hpp"
#include "SimpleIni.h"
#include "Map.hpp"

#include <string>
#include <memory>

class ProtoMapper
{
	const std::string _title = "ProtoMapper - ";
	bool _appRunning = true, _mapOpen = true, _panning = false, _fullscreen = true, _configUpdate = false;
	
	std::unique_ptr<Map> _currentMap;

	float _fWidth = 1024.f, _fHeight = 768.f;
	int _wWidth = 1024, _wHeight = 768;

	SDL_Window* _window = nullptr;
	SDL_GLContext _context;
	SDL_Texture* _mapSprite = nullptr;

	std::string _configFile;
	CSimpleIniA _configData;

public:
	ProtoMapper();
	~ProtoMapper();

	void CreateToolBar();

	bool Configure();
	void Run();
	bool Update(float);
	bool Draw();
};

#endif