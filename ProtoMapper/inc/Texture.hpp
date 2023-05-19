#ifndef PROTOMAPPER_TEXTURE_HPP
#define PROTOMAPPER_TEXTURE_HPP

extern "C"
{
	struct SDL_Surface;
}

/*
	Convenience functions.
*/

SDL_Surface* CreateImageBlank(int width, int height);
SDL_Surface* CreateImageFromPNG(const char* file);
void FreeImageSurface(SDL_Surface* image);


/*
	This 2D Texture format is designed to be used with SDL_Surfaces.
*/
class Texture2D
{
#ifdef USE_GLES
	using IDType = unsigned short;
#else
	using IDType = unsigned int;
#endif

	IDType _ID = 0u;

public:
	Texture2D() = default;

	Texture2D& Create();
	Texture2D& Destroy();
	void Bind(unsigned int slot = 0u) const;
	void Unbind() const;
	Texture2D& WriteImage(SDL_Surface* image);
	Texture2D& GenerateBlank(int w, int h, unsigned int colorValue = 0xFFFFFFFF);

	IDType Target() const;
	IDType ID() const;
};

#endif