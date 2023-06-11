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

ResourceManager::ResourceManager()
    : _stringMap(std::pmr::map<const std::pmr::string, std::pmr::string>(std::pmr::polymorphic_allocator(&_textAllocator)))
{

}

ResourceManager::~ResourceManager()
{
    _textures.clear();
    _shaders.clear();
    _stringMap.clear();
    _textureRefCount.clear();
    _shaderRefCount.clear();
}

std::string_view ResourceManager::LoadString(const std::string_view& name, const std::string_view& content)
{
    auto& str = _stringMap.insert_or_assign(
        std::pmr::string{name, std::pmr::polymorphic_allocator(&_textAllocator)},
        std::pmr::string{content, std::pmr::polymorphic_allocator(&_textAllocator)}
        );

    return std::string_view{str.first->second.c_str(), str.first->second.size()};
}

std::string_view ResourceManager::GetString(const std::string_view& name)
{
    if (_stringMap.count(name.data()) > 0)
    {
        auto& result = _stringMap.at(name.data());
        
        std::string_view view { result.c_str(), result.size() };

        return view;
    }

    return std::string_view{};
}

void ResourceManager::UnloadString(const std::string_view& name)
{
    _stringMap.erase(name.data());
}

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