#include "ProtoMapper.hpp"


void ProtoMapper::Run()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), std::string{ "ProtoMapper " } + VERSION_NUMBER);

	while (_appRunning)
	{

		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				_appRunning = false;
				break;
			}
			}
		}

	}

	window.close();
}


int main()
{
	ProtoMapper mapper;

	mapper.Run();
}