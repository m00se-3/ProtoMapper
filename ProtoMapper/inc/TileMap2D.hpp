#ifndef MAPPER_TILEMAP2D_HPP
#define MAPPER_TILEMAP2D_HPP

#include "Map.hpp"
#include <random>

class TileMap2D : public Map
{
	std::mt19937 _generator;
	sf::Image _data;

public:

	TileMap2D() = default;
	~TileMap2D() = default;

	bool Create(unsigned int, unsigned int) override;
	bool Generate(size_t) override;
};

#endif // !MAPPER_TILEMAP2D_HPP
