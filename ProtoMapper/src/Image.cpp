#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(const std::filesystem::path& filename)
{
	Load(filename);
}

Image::~Image() { stbi_image_free(_data); }

void Image::Load(const std::filesystem::path& filename)
{
	// For now, we will require png images.
	if (std::filesystem::exists(filename) && filename.extension() == ".png")
	{
		_data = stbi_load(filename.string().c_str(), &_width, &_height, &_channels, 0);
	}
}

void Image::LoadCopy(int w, int h, uint8_t* ptr = nullptr)
{
	Destroy();

	_width = w; _height = h;

	size_t size = (size_t)(w * h);

	_data = (uint8_t*)malloc(size);

	if (!ptr)
	{
		memset(_data, 0u, size);
	}
	else
	{
		memcpy(_data, ptr, size);
	}
}

uint8_t* Image::Data() const { return _data; }

int Image::Width() const
{
	return _width;
}

int Image::Height() const
{
	return _height;
}

void Image::Destroy()
{
	if (_data)
	{
		free(_data);
	}
}
