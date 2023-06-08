#ifndef PROTOMAPPER_IMAGE_HPP
#define PROTOMAPPER_IMAGE_HPP

#include <filesystem>

// Forward declaration.
class ResourceManager;

/*
	Image container class.

	Objects of this type are not, as of yet, handled by the ResourceManager.
*/
class Image
{
	int _width = 0, _height = 0, _channels = 4;
	uint8_t* _data = nullptr;

public:
	Image() = default;
	Image(const std::filesystem::path& filename);
	~Image();

	void Create(int w, int h, uint8_t* data = nullptr);

	uint8_t* Data() const;
	int Width() const;
	int Height() const;

};

#endif // !PROTOMAPPER_IMAGE_HPP
