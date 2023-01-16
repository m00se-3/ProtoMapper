#include "TileMap2D.hpp"

bool TileMap2D::Create(unsigned int w, unsigned int h)
{
	_data.create(w, h, sf::Color{ 255u, 255u, 255u });
	_texture.create(w, h);

	return true;
}

bool TileMap2D::Generate(size_t seed)
{
	_generator.seed(seed);

	auto sizeVec = _data.getSize();

	for (unsigned int y = 0u; y < sizeVec.y; ++y)
	{
		for (unsigned int x = 0u; x < sizeVec.x; ++x)
		{
			auto result = _generator();
			
			_data.setPixel(x, y, sf::Color{ uint8_t(result >> 24), uint8_t(result >> 16), uint8_t(result >> 8) });
		}
	}

	_texture.update(_data);

	return true;
}