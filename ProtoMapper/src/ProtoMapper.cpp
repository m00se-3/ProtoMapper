#include "ProtoMapper.hpp"
#include "SFML/Window/Event.hpp"
#include "HeightMap2D.hpp"
#include "TopMenu.hpp"

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
	: _currentMap(std::make_unique<HeightMap2D>())
{
	_currentMap->Create(_fWidth, _fHeight);
	_mapSprite.setTexture(_currentMap->GetTexture());
}

ProtoMapper::~ProtoMapper() { _currentMap.reset(); }


void ProtoMapper::Run()
{
	_window.create(sf::VideoMode(_windowWidth, _windowHeight), std::string{ "ProtoMapper - " } + VERSION_NUMBER);

	_camera.setViewport(sf::FloatRect{ 0.f, 0.05f, 1.f, 0.95f });

	_window.setView(_camera);
	_window.setFramerateLimit(30);

	_rootGui.setTarget(_window);

	TopMenu bar(_fWidth * 1.f, _fHeight * 0.05f);
	_rootGui.add(bar.GetGroup());

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
			case sf::Event::MouseButtonPressed:
			{
				if (event.mouseButton.button == sf::Mouse::Middle) _panning = true;
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				if (event.mouseButton.button == sf::Mouse::Middle) _panning = false;
				break;
			}
			case sf::Event::MouseWheelScrolled:
			{

				break;
			}
			case sf::Event::MouseMoved:
			{

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