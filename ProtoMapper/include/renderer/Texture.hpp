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
#ifndef PROTO_TEXTURE_HPP
#define PROTO_TEXTURE_HPP

#include <cstdint>

#include "Image.hpp"

namespace proto
{
    struct Texture2D
	{
		using IDType = uint32_t;

		IDType ID = 0u;

		Texture2D() = default;
		Texture2D(const Texture2D&);
		explicit Texture2D(IDType id);
		~Texture2D();

		bool operator==(const Texture2D& rhs) const;
		Texture2D& operator=(const Texture2D& rhs);

		Texture2D& Create();

		bool Valid() const; // Is the Texture valid.

		// Resets the current reference object to 0.
		void Reset();

		IDType GetID() const;

		void Destroy();
		void Bind(uint32_t slot = 0u);
		void Bind(uint32_t slot = 0u) const;
		void Unbind();
		void Unbind() const;
		Texture2D& WriteImage(const Image& img);
		Texture2D& WriteData(const void* data, int width, int height);
		Texture2D& GenerateBlank(int w, int h, uint32_t colorValue = 0xFFFFFFFF);

		IDType Target() const;

	};
}

#endif