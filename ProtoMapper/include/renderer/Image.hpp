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
#ifndef PROTO_IMAGE_HPP
#define PROTO_IMAGE_HPP

#include <cstdint>
#include <filesystem>
#include <vector>
#include <span>

namespace proto
{
    /*
		Image container class.
	*/
	class Image
	{
	public:
		Image() = default;
		Image(const std::filesystem::path& filename);
		Image(uint32_t w, uint32_t h, std::span<uint8_t> dataIn);

		// Load a fresh image from a file.
		void Load(const std::filesystem::path& filename);

		// Copy image data from an existing memory buffer.
		// This will erase any data already contained in the Image buffer.
		// Passing null pointer will zero-initialize the image buffer to requested size.
		void LoadCopy(uint32_t w, uint32_t h, std::span<uint8_t> dataIn);

		[[nodiscard]] constexpr auto Data(this auto&& self) { return std::span<uint8_t>(self._data.begin(), self._data.end()); }
		[[nodiscard]] constexpr auto&& Width(this auto&& self) { return self._width; }
		[[nodiscard]] constexpr auto&& Height(this auto&& self) { return self._height; }
		[[nodiscard("The boolean returned from Image::Empty() has been ignored.")]] bool Empty() const { return _data.empty(); }

	private:
		uint32_t _width, _height;
		std::vector<uint8_t> _data;

	};
}

#endif
