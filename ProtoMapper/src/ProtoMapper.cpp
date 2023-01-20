#include "ProtoMapper.hpp"
#include "SFML/Window/Event.hpp"
#include "HeightMap2D.hpp"
#include "TopMenu.hpp"

#include <chrono>
#include <filesystem>

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
	: _currentMap()
{
	
}

ProtoMapper::~ProtoMapper() 
{
	if (_configUpdate)
	{
		_configData.SaveFile(_configFile.c_str());
	}

	_configData.Reset();
	_currentMap.reset();
}

bool ProtoMapper::Configure()
{
#if defined(_DEBUG_) || defined(_RELEASE_)

	_configFile = ASSETS_DIR;
	_configFile += "/assets/config/config.ini";

#else

	_configFile = "./assets/config/config.ini"

#endif // _DEBUG_

		if (std::filesystem::exists(_configFile))
		{
			auto err = _configData.LoadFile(_configFile.c_str());

			if (err < 0) return false;
		}
		else
			return false;


	auto width = _configData.GetLongValue("display", "width", -1);
	auto height = _configData.GetLongValue("display", "height", -1);

	if (width < 0 || height < 0)
		return false;

	/*
		If one of the dimensions is zero, it's safe to assume the native display is being used.
	*/
	if (width != 0)
	{
		_videoMode.width = static_cast<unsigned int>(width);
		_videoMode.height = static_cast<unsigned int>(height);
		_fullscreen = false;
	}

	return true;
}


void ProtoMapper::Run()
{
	if (_fullscreen)
		_window.create(sf::VideoMode::getDesktopMode(), std::string{"ProtoMapper - "} + VERSION_NUMBER);
	else
		_window.create(_videoMode, std::string{ "ProtoMapper - " } + VERSION_NUMBER);

	_camera.setViewport(sf::FloatRect{ 0.f, 0.05f, 1.f, 0.95f });

	_window.setView(_camera);
	_window.setFramerateLimit(30);

	_rootGui.setTarget(_window);

	TopMenu bar(this, _fWidth * 1.f, _fHeight * 0.05f);
	_rootGui.add(bar.GetGroup());


	while (_appRunning)
	{
		sf::Clock clock;
		sf::Event event;
		while (_window.pollEvent(event))
		{
			if (_rootGui.handleEvent(event)) continue;
			
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
			case sf::Event::Resized:
			{
				_videoMode.width = event.size.width;
				_videoMode.height = event.size.height;

				auto err1 = _configData.SetLongValue("display", "width", event.size.width);
				auto err2 = _configData.SetLongValue("display", "height", event.size.height);

				if (err1 != SI_UPDATED || err2 != SI_UPDATED)
				{
					fprintf_s(stdout, "New display values failed to save. Possibly a corrupt config file.");
					return;
				}

				_configUpdate = true;

				break;
			}
			}
		}

		if (!Update(clock.restart().asSeconds())) break;

		_window.clear(sf::Color{ 50u, 50u, 50u, 50u });

		if (!Draw()) break;

		_rootGui.draw();
		_window.display();

	}

	_window.close();
}


int main()
{
	ProtoMapper mapper;

	if(mapper.Configure())
		mapper.Run();
	else
		fprintf_s(stdout, "Failed to load application configurations. Please reinstall the program.");
}