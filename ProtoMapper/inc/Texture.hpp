#ifndef PROTOMAPPER_TEXTURE_HPP
#define PROTOMAPPER_TEXTURE_HPP

extern "C"
{
	struct SDL_Surface;
}

/*
	The following free functions are created to abstract the SDL_Surface type
	from the rest of the API. This way, SDL_Surface's are not easily messed with
	by any other code.
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
	void Bind() const;
	void Unbond() const;
	Texture2D& WriteImage(SDL_Surface* image);

	IDType Target() const;
};

#endif