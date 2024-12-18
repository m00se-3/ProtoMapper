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
#include <Texture.hpp>

namespace proto
{
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

	void Texture2D::Reset() { ID = 0u; }

	void Texture2D::Destroy() { glDeleteTextures(1, &ID); }

	void Texture2D::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

	Texture2D& Texture2D::WriteImage(Image& img)
	{
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, static_cast<int>(img.Width()), static_cast<int>(img.Height()), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.Data().data());
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

	Texture2D& Texture2D::GenerateBlank(int w, int h, uint32_t colorValue)
	{
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colorValue);
		Unbind();
		return *this;
	}
}
