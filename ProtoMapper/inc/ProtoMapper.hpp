#ifndef PROTOMAPPER_MAIN_HPP
#define PROTOMAPPER_MAIN_HPP

#include "SFML/Window.hpp"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/TGUI.hpp"
#include "Map.hpp"

#include <string>
#include <memory>

class ProtoMapper
{
	bool _appRunning = true, _mapOpen = true;
	
	std::unique_ptr<Map> _currentMap;
	tgui::GuiSFML _rootGui;
	tgui::Group::Ptr _toolBar;

	unsigned int _windowWidth = 800u, _windowHeight = 600u;
	float _fWidth = 800.f, _fHeight = 600.f;

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