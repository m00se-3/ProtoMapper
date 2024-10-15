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
#include "Image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace proto
{
	
	Image::Image(const std::filesystem::path& filename)
	: _width(), _height()
	{
		Load(filename);
	}

	Image::Image(uint32_t w, uint32_t h, std::span<uint8_t> dataIn)
		: _width(w), _height(h), _data(dataIn.begin(), dataIn.end())
	{
	}

	void Image::Load(const std::filesystem::path& filename)
	{
		// For now, we will require png images.
		if (std::filesystem::exists(filename) && filename.extension() == ".png")
		{
			int w{}, h{}, c{};
			uint8_t* ptr = stbi_load(filename.string().c_str(), &w, &h, &c, 0);

			const auto size = size_t(w) * size_t(h);
			auto sp = std::span<uint8_t>{ ptr, size };
			_data.assign(sp.begin(), sp.end());
		}
	}

	void Image::LoadCopy(uint32_t w, uint32_t h, std::span<uint8_t> dataIn)
	{
		_width = w; _height = h;
		_data.clear();

		_data.reserve(size_t(w) * size_t(h));

		if (!dataIn.empty())
		{
			_data.assign(dataIn.begin(), dataIn.end());
		}
	}

}
