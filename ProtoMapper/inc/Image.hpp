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

	// Load a fresh image from a file.
	void Load(const std::filesystem::path& filename);

	// Copy image data from an existing memory buffer.
	// This will erase any data already contained in the Image buffer.
	void LoadCopy(int w, int h, uint8_t* ptr = nullptr);

	uint8_t* Data() const;
	int Width() const;
	int Height() const;

	void Destroy();

};

#endif // !PROTOMAPPER_IMAGE_HPP
