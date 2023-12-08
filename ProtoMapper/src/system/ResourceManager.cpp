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

namespace proto
{
    template<> ReferenceCounter<Shader>* GPUResource<Shader>::_manager = nullptr;
    template<> ReferenceCounter<Texture2D>* GPUResource<Texture2D>::_manager = nullptr;
	
	ResourceManager::ResourceManager(const std::span<uint8_t>& resource)
        : _upstream(resource.data(), resource.size()), _textAllocator(&_upstream), 
        _textures(std::make_unique<ReferenceCounter<Texture2D>>()), _shaders(std::make_unique<ReferenceCounter<Shader>>())
    {
        GPUResource<Texture2D>::SetManager(_textures.get());
        GPUResource<Shader>::SetManager(_shaders.get());
    }

    ReferenceCounter<Texture2D>* ResourceManager::Textures() { return _textures.get(); }

    ReferenceCounter<Shader>* ResourceManager::Shaders() { return _shaders.get(); }

    void ResourceManager::LoadStringRes(const std::string& name, const std::string& content)
    {
        auto str = _stringMap.emplace(
            std::pmr::string{ name, &_textAllocator },
            std::pmr::string{ content, &_textAllocator }
        );
    }

    std::string_view ResourceManager::GetString(const std::string_view& name)
    {
        if (_stringMap.contains(name.data()))
        {
            auto& result = _stringMap.at(std::pmr::string{ name.data(), name.size(), &_textAllocator });

            std::string_view view{ result.c_str(), result.size() };

            return view;
        }

        return std::string_view{};
    }

    void ResourceManager::UnloadString(const std::string_view& name)
    {
        _stringMap.erase(std::pmr::string{ name.data(), name.size(), &_textAllocator });
    }
}
