#ifndef MAPPER_TILEMAP2D_HPP
#define MAPPER_TILEMAP2D_HPP

#include "Map.hpp"
#include "Texture.hpp"
#include <random>

extern "C"
{
	struct SDL_Surface;
}

class HeightMap2D : public Map
{
	std::mt19937 _generator;
	SDL_Surface* _data = nullptr;
	Texture2D _texture;

public:

	HeightMap2D() = default;
	~HeightMap2D() = default;

	bool Create(ProtoMapper* par, unsigned int, unsigned int) override;
	bool Generate(size_t) override;
};

#endif // !MAPPER_TILEMAP2D_HPP
