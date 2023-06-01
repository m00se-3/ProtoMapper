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

Texture2D::Texture2D(IDType id)
	:ID(id)
{

}

Texture2D& Texture2D::Create()
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return *this;
}

void Texture2D::Destroy() { glDeleteTextures(1, &ID);  }

void Texture2D::Bind(unsigned int slot) 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture2D::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture2D& Texture2D::WriteImage(SDL_Surface* image)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, image->w, image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	Unbind();
	return *this;
}

Texture2D& Texture2D::WriteData(const void* data, int width, int height)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	Unbind();
	return *this;
}

Texture2D& Texture2D::GenerateBlank(int w, int h, unsigned int colorValue)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colorValue);
	Unbind();
	return *this;
}

Texture2D::IDType Texture2D::Target() const { return GL_TEXTURE_2D; }

