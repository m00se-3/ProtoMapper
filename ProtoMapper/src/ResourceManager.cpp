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

#include "ResourceManager.hpp"

template<>
std::unordered_map<const std::string, Texture2D>& ResourceManager::GetStorageMap(const Texture2D&) { return _textures; }

template<>
std::unordered_map<const std::string, Shader>& ResourceManager::GetStorageMap(const Shader&) { return _shaders; }

template<>
std::unordered_map<ResourceManager::IDType, uint16_t>& ResourceManager::GetReferenceMap(const Texture2D&) { return _textureRefCount; }

template<>
std::unordered_map<ResourceManager::IDType, uint16_t>& ResourceManager::GetReferenceMap(const Shader&) { return _shaderRefCount; }

template<>
void ResourceManager::DestroyResource(IDType id, Texture2D temp)
{
	temp.ID = id;
	temp.Destroy();
}

template<>
void ResourceManager::DestroyResource(IDType id, Shader temp)
{
	temp.ID = id;
	temp.Destroy();
}