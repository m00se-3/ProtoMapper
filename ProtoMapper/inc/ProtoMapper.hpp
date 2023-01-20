#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/Core.hpp"
#include "TGUI/Backends/SFML.hpp"
#include "SimpleIni.h"
#include "Map.hpp"

#include <string>
#include <memory>

class ProtoMapper
{
	bool _appRunning = true, _mapOpen = true, _panning = false, _fullscreen = true, _configUpdate = false;
	
	std::unique_ptr<Map> _currentMap;
	tgui::GuiSFML _rootGui;

	sf::VideoMode _videoMode = sf::VideoMode{ 1024u, 768u };
	float _fWidth = 1024.f, _fHeight = 768.f;

	sf::RenderWindow _window;
	sf::Sprite _mapSprite;
	sf::View _camera;

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