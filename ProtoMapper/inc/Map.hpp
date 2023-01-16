#ifndef MAPPER_MAP_HPP
#define MAPPER_MAP_HPP

#include "SFML/Graphics.hpp"


class Map
{
	
protected:
	sf::Texture _texture;

public:
	Map() = default;
	virtual ~Map() = default;

	virtual bool Create(unsigned int, unsigned int) = 0;	// The size of the map in cell width and height. 
	virtual bool Generate(size_t) = 0;

	sf::Texture& GetTexture();
};

#endif