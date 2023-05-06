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

Texture2D& Texture2D::Create()
{
	glGenTextures(1, &_ID);
	return *this;
}

Texture2D& Texture2D::Destroy() { glDeleteTextures(1, &_ID); return *this;  }

void Texture2D::Bind() const { glBindTexture(GL_TEXTURE_2D, _ID); }

void Texture2D::Unbond() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture2D& Texture2D::WriteImage(SDL_Surface* image)
{
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32UI, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, image->pixels);
	return *this;
}

Texture2D::IDType Texture2D::Target() const { return GL_TEXTURE_2D; }