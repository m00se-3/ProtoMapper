#include "HeightMap2D.hpp"
#include "ProtoMapper.hpp"

bool HeightMap2D::Create(ProtoMapper* par, unsigned int w, unsigned int h)
{
	_parent = par;
	_data = SDL_CreateRGBSurface(0u, w, h, 8, 255u, 255u, 255u, 255u);
	_texture.Create();

	return true;
}

bool HeightMap2D::Generate(size_t seed)
{
	_generator.seed(seed);

	SDL_LockSurface(_data);

	for (int y = 0; y < _data->h; ++y)
	{
		for (int x = 0; x < _data->w; ++x)
		{
			auto result = _generator();
			
			const SDL_Rect rect = SDL_Rect{x, y, 1, 1};
			
			SDL_FillRect(_data, &rect, result);
		}
	}

	SDL_UnlockSurface(_data);

	_texture.WriteImage(_data);

	return true;
}