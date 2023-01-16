#include "ProtoMapper.hpp"
#include "TileMap2D.hpp"

#include <chrono>

bool ProtoMapper::Update(float dt)
{


	return true;
}

bool ProtoMapper::Draw()
{
	if (_mapOpen) _window.draw(_mapSprite);

	return true;
}


ProtoMapper::ProtoMapper()
	:_toolBar(tgui::Group::create())
{
	CreateToolBar();
	_currentMap.reset(new TileMap2D{});
	_currentMap->Create(_windowWidth, _windowHeight);
	_mapSprite.setTexture(_currentMap->GetTexture());
}

ProtoMapper::~ProtoMapper() { _currentMap.reset(); }

void ProtoMapper::CreateToolBar()
{
	_toolBar->setPosition(tgui::Layout2d{});
	_toolBar->setSize(_windowWidth, _windowHeight / 6u);

	_toolBar->add(tgui::Button::create("Click Me!"), "btn001");
}

void ProtoMapper::Run()
{
	_window.create(sf::VideoMode(_windowWidth, _windowHeight), std::string{ "ProtoMapper - " } + VERSION_NUMBER);

	_camera.setSize(_fWidth, _fHeight);

	_window.setView(_camera);
	_window.setFramerateLimit(30);

	_rootGui.setTarget(_window);
	_rootGui.add(_toolBar, "Tool Bar");

	_currentMap->Generate(std::chrono::high_resolution_clock::now().time_since_epoch().count());

	while (_appRunning)
	{
		sf::Clock clock;
		sf::Event event;
		while (_window.pollEvent(event))
		{
			_rootGui.handleEvent(event);
			
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				_appRunning = false;
				break;
			}
			}
		}

		if (!Update(clock.restart().asSeconds())) break;

		_window.clear();

		if (!Draw()) break;

		_rootGui.draw();
		_window.display();

	}

	_window.close();
}


int main()
{
	ProtoMapper mapper;

	mapper.Run();
}