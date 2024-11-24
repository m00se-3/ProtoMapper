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

#include <glad/glad.h>
#include <Image.hpp>
namespace proto
{
    inline constexpr auto TextureWhite = 0xFFFFFFFF;

    struct Texture2D
    {
	using IDType = uint32_t;

	IDType ID = 0u;

	Texture2D() = default;
	explicit Texture2D(IDType id);

	bool operator==(const Texture2D& rhs) const = default;

	Texture2D& Create();

	[[nodiscard]] constexpr bool Valid() const { return ID != 0; } // Is the Texture valid.

	// Resets the current reference object to 0.
	void Reset();

	[[nodiscard]] constexpr IDType GetID() const { return ID; }

	void Bind(this auto&& self, uint32_t slot = 0u)
	{
	    glActiveTexture(GL_TEXTURE0 + slot);
	    glBindTexture(GL_TEXTURE_2D, self.ID);
	}

	void Destroy();
	static void Unbind();
	Texture2D& WriteImage(Image& img);
	Texture2D& WriteData(const void* data, int width, int height);
	Texture2D& GenerateBlank(int w, int h, uint32_t colorValue = TextureWhite);

	[[nodiscard]] static constexpr IDType Target() { return GL_TEXTURE_2D; }

    };
}

#endif
