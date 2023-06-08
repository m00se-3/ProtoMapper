#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Image::Image(const std::filesystem::path& filename)
{
	// For now, we will require png images.
	if (std::filesystem::exists(filename) && filename.extension() == ".png")
	{
		_data = stbi_load(filename.string().c_str(), &_width, &_height, &_channels, 0);
	}
}

Image::~Image() { stbi_image_free(_data); }

void Image::Create(int w, int h, uint8_t* data)
{
	_width = w; _height = h; _data = data;
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
