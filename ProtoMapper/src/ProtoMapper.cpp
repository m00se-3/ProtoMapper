#include "ProtoMapper.hpp"

bool ProtoMapper::Update(float dt)
{


	return true;
}

bool ProtoMapper::Draw()
{


	return true;
}


ProtoMapper::ProtoMapper()
	:_toolBar(tgui::Group::create())
{
	CreateToolBar();
}

void ProtoMapper::CreateToolBar()
{
	_toolBar->setPosition(tgui::Layout2d{});
	_toolBar->setSize(_windowWidth, _windowHeight / 6u);

	_toolBar->add(tgui::Button::create("Click Me!"), "btn001");
}

void ProtoMapper::Run()
{
	sf::RenderWindow window(sf::VideoMode(_windowWidth, _windowHeight), std::string{ "ProtoMapper - " } + VERSION_NUMBER);

	_camera.setSize(_fWidth, _fHeight);

	window.setFramerateLimit(30);

	_rootGui.setTarget(window);
	_rootGui.add(_toolBar, "Tool Bar");

	while (_appRunning)
	{
		sf::Clock clock;
		sf::Event event;
		while (window.pollEvent(event))
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

		window.clear();

		if (!Draw()) break;

		_rootGui.draw();
		window.display();

	}

	window.close();
}


int main()
{
	ProtoMapper mapper;

	mapper.Run();
}