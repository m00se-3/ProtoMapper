/*
	ProtoMapper - Map creation and pathfinding software for game development.
	Copyright (C) 2023  Samuel Bridgham - moosethree473@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
module;

#include <cstdint>
#include <filesystem>

#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

export module proto.Image;

namespace proto
{
    /*
		Image container class.
	*/
	export class Image
	{
	public:
		Image() = default;
		Image(const std::filesystem::path& filename);
		Image(int w, int h, uint8_t* ptr = nullptr);
		~Image();

		// Load a fresh image from a file.
		void Load(const std::filesystem::path& filename);

		// Copy image data from an existing memory buffer.
		// This will erase any data already contained in the Image buffer.
		// Passing null pointer will zero-initialize the image buffer to requested size.
		void LoadCopy(int w, int h, uint8_t* ptr = nullptr);

		uint8_t* Data() const;
		int Width() const;
		int Height() const;
		[[nodiscard("The boolean returned from Image::Empty() has been ignored.")]] bool Empty() const;

		void Destroy();

	private:
		int _width = 0, _height = 0, _channels = 4;
		uint8_t* _data = nullptr;

	};
}