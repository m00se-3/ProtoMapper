#include "Texture.hpp"

#include "glad/glad.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_image.h"

SDL_Surface* CreateImageBlank(int width, int height)
{
	return SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 255u);
}

SDL_Surface* CreateImageFromPNG(const char* file)
{
	return IMG_Load(file);
}

void FreeImageSurface(SDL_Surface* image) { SDL_FreeSurface(image); }

void Texture2D::Create()
{
	glGenTextures(1, &_ID);
}

void Texture2D::Destroy() { glDeleteTextures(1, &_ID); }

void Texture2D::Bind() const { glBindTexture(GL_TEXTURE_2D, _ID); }

void Texture2D::WriteImage(SDL_Surface* image)
{
	
}