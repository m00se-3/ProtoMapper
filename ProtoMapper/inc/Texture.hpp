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

#ifndef PROTOMAPPER_TEXTURE_HPP
#define PROTOMAPPER_TEXTURE_HPP

#include "Image.hpp"

class ResourceManager;


/*
	This 2D Texture format is designed to be used with SDL_Surfaces.
*/
struct Texture2D
{
#ifdef USE_GLES
	using IDType = unsigned short;
#else
	using IDType = unsigned int;
#endif
	IDType ID = 0u;

	Texture2D() = default;
	Texture2D(const Texture2D&);
	explicit Texture2D(IDType id);
	~Texture2D();

	bool operator==(const Texture2D& rhs);
	bool operator==(const Texture2D& rhs) const;
	Texture2D& operator=(const Texture2D& rhs);

	Texture2D& Create();
	void Destroy();
	void Bind(unsigned int slot = 0u);
	void Bind(unsigned int slot = 0u) const;
	void Unbind();
	void Unbind() const;
	Texture2D& WriteImage(const Image& img);
	Texture2D& WriteData(const void* data, int width, int height);
	Texture2D& GenerateBlank(int w, int h, unsigned int colorValue = 0xFFFFFFFF);

	IDType Target() const;

	static void SetResourceManager(ResourceManager* ptr);


private:
	static ResourceManager* _manager;
};

#endif