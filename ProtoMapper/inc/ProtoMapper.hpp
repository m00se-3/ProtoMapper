#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/Core.hpp"
#include "TGUI/Backends/SFML.hpp"
#include "Map.hpp"

#include <string>
#include <memory>

class ProtoMapper
{
	bool _appRunning = true, _mapOpen = true, _panning = false;
	
	std::unique_ptr<Map> _currentMap;
	tgui::GuiSFML _rootGui;

	unsigned int _windowWidth = 1024u, _windowHeight = 768u;
	float _fWidth = 1024.f, _fHeight = 768.f;

	sf::RenderWindow _window;
	sf::Sprite _mapSprite;
	sf::View _camera;

public:
	ProtoMapper();
	~ProtoMapper();

	void CreateToolBar();

	void Run();
	bool Update(float);
	bool Draw();
};

#endif