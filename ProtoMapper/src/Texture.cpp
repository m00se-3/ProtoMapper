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

#include "Texture.hpp"
#include "ResourceManager.hpp"

#include "glad/glad.h"

Texture2DManager* Texture2D::_manager = nullptr;

void Texture2D::SetResourceManager(Texture2DManager* ptr)
{
	_manager = ptr;
}



Texture2D::Texture2D(const Texture2D& other)
	:ID(other.ID)
{
	_manager->AddReference(ID);
}

Texture2D::Texture2D(IDType id)
	:ID(id)
{
	_manager->AddReference(ID);
}

bool Texture2D::operator==(const Texture2D& rhs)
{
	return ID == rhs.ID;
}

bool Texture2D::operator==(const Texture2D& rhs) const
{
	return ID == rhs.ID;
}

Texture2D& Texture2D::operator=(const Texture2D& rhs)
{
	if (ID != 0u) _manager->SubReference(ID);

	ID = rhs.ID;
	_manager->AddReference(ID);

	return *this;
}

Texture2D& Texture2D::Create()
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	_manager->AddReference(ID);

	return *this;
}

void Texture2D::Reset() { ID = 0u; }

void Texture2D::Destroy() { glDeleteTextures(1, &ID);  }

void Texture2D::Bind(unsigned int slot) 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture2D::Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

void Texture2D::Unbind() const { glBindTexture(GL_TEXTURE_2D, 0); }

Texture2D& Texture2D::WriteImage(const Image& img)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.Width(), img.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.Data());
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

Texture2D& Texture2D::GenerateBlank(int w, int h, unsigned int colorValue)
{
	Bind();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, &colorValue);
	Unbind();
	return *this;
}

Texture2D::IDType Texture2D::Target() const { return GL_TEXTURE_2D; }


Texture2D::~Texture2D() { _manager->SubReference(ID); }

