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

ResourceManager::ResourceManager(void* memory, size_t size)
    : _textResource(memory, size), _textAllocator(&_textResource), _stringMap(std::pmr::map<std::pmr::string, std::pmr::string>(&_textAllocator))
{

}

ResourceManager::~ResourceManager()
{    
    _textures.clear();
    _shaders.clear();
    _stringMap.clear();
    _textAllocator.release();
}

void ResourceManager::Lock()
{
    _mutex.lock();
}

void ResourceManager::Unlock()
{
    _mutex.unlock();
}

void ResourceManager::Shared_Lock()
{
    _mutex.lock_shared();
}

void ResourceManager::Shared_Unlock()
{
    _mutex.unlock_shared();
}

std::string_view ResourceManager::LoadString(const std::string& name, const std::string& content)
{
    auto str = _stringMap.emplace(
        std::pmr::string{name, &_textAllocator},
        std::pmr::string{content, &_textAllocator}
        );

    return std::string_view{str.first->second.c_str(), str.first->second.size()};
}

std::string_view ResourceManager::GetString(const std::string_view& name)
{
    if (_stringMap.count(name.data()) > 0)
    {
        auto& result = _stringMap.at(std::pmr::string{ name.data(), name.size(), &_textAllocator });
        
        std::string_view view { result.c_str(), result.size() };

        return view;
    }

    return std::string_view{};
}

void ResourceManager::UnloadString(const std::string_view& name)
{
    _stringMap.erase(std::pmr::string{ name.data(), name.size(), &_textAllocator });
}

Texture2D ResourceManager::LoadTexture(const std::string_view& name)
{
    if (_textures.count(std::string{name.data(), name.size()}) > 0)
    {
        auto& res = _textures.at(std::string{name.data(), name.size()});

        return Texture2D{ res };
    }

    auto result = _textures.emplace(std::string{name.data(), name.size()}, Texture2D{});

    return result.first->second;
}

Texture2D ResourceManager::GetTexture(const std::string_view& name)
{
    if (_textures.count(std::string{name.data(), name.size()}) > 0)
    {
        return _textures.at(std::string{name.data(), name.size()});
    }

    return Texture2D{};
}

void ResourceManager::UnloadTexture(const std::string_view& name)
{
    /*
        When we unload a resource, we flag it for removal until the
        las reference to it is destroyed.
    */
    
    if (_textures.count(std::string{name.data(), name.size()}) > 0)
    {
        auto& texture = _textures.at(std::string{name.data(), name.size()});

        auto& reference = _textureRefCount.at(texture.ID);
        reference.first = true;
        _textures.erase(std::string{name.data(), name.size()});
    }
}

void ResourceManager::AddRefTexture(IDType id)
{
    if (_textureRefCount.count(id) == 0u)
    {
        _textureRefCount.insert_or_assign(id, std::make_pair(false, 1u));
    }
    else
    {
        auto& count = _textureRefCount.at(id);
        ++count.second;
    }
}

void ResourceManager::SubRefTexture(IDType id)
{
    if (_textureRefCount.count(id) > 0u)
    {
        auto& count = _textureRefCount.at(id);
        --count.second;

        if (count.first && count.second == 0u)
        {
            auto tex = Texture2D{};
            tex.ID = id;
            tex.Destroy();
        }
    }
}

Shader ResourceManager::LoadShader(const std::string_view& name)
{
    if (_shaders.count(std::string{ name.data(), name.size() }) > 0)
    {
        auto& res = _shaders.at(std::string{name.data(), name.size()});

        return Shader{ res };
    }

    auto result = _shaders.emplace(name, Shader{});

    return result.first->second;
}

Shader ResourceManager::GetShader(const std::string_view& name)
{
    if (_shaders.count(std::string{ name.data(), name.size() }) > 0)
    {
        return _shaders.at(std::string{ name.data(), name.size() });
    }

    return Shader{};
}

void ResourceManager::UnloadShader(const std::string_view& name)
{
    /*
        When we unload a resource, we flag it for removal until the
        las reference to it is destroyed.
    */
    
    if (_shaders.count(std::string{ name.data(), name.size() }) > 0)
    {
        auto& shader = _shaders.at(std::string{name.data(), name.size()});

        auto& reference = _shaderRefCount.at(shader.ID);
        reference.first = true;
        _shaders.erase(std::string{name.data(), name.size()});
    }
}

void ResourceManager::AddRefShader(IDType id)
{
    if (_shaderRefCount.count(id) == 0u)
    {
        _shaderRefCount.insert_or_assign(id, std::make_pair(false, 1u));
    }
    else
    {
        auto& count = _shaderRefCount.at(id);
        ++count.second;
    }
}

void ResourceManager::SubRefShader(IDType id)
{
    if (_shaderRefCount.count(id) > 0u)
    {
        auto& count = _shaderRefCount.at(id);
        --count.second;

        if (count.first && count.second == 0u)
        {
            auto shad = Shader{};
            shad.ID = id;
            shad.Destroy();
        }
    }
}
