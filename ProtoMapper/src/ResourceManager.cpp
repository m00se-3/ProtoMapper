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

    ResourceManager::ResourceManager(void* memory, size_t size)
        : _textResource(memory, size), _textAllocator(&_textResource), _stringMap(std::pmr::map<std::pmr::string, std::pmr::string>(&_textAllocator)),
        _textures(std::make_unique<Texture2DManager>()), _shaders(std::make_unique<ShaderManager>())
    {

    }

    ResourceManager::~ResourceManager()
    {
        _stringMap.clear();
        _textAllocator.release();
    }

    Texture2DManager* ResourceManager::Textures() { return _textures.get(); }

    ShaderManager* ResourceManager::Shaders() { return _shaders.get(); }

    void ResourceManager::Lock()
    {
        _mutex.lock();
    }

    void ResourceManager::Unlock()
    {
        _mutex.unlock();
    }

    std::string_view ResourceManager::LoadString(const std::string& name, const std::string& content)
    {
        auto str = _stringMap.emplace(
            std::pmr::string{ name, &_textAllocator },
            std::pmr::string{ content, &_textAllocator }
        );

        return std::string_view{ str.first->second.c_str(), str.first->second.size() };
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

    Texture2DManager::~Texture2DManager()
    {
        _references.clear();
        _storage.clear();
    }

    Texture2D Texture2DManager::Load(const std::string_view& name)
    {
        m_Storage.lock();

        if (_storage.contains(name.data()))
        {
            auto& res = _storage.at(std::string{ name.data(), name.size() });

            m_Storage.unlock();

            return Texture2D{ res };
        }

        auto result = _storage.emplace(std::string{ name.data(), name.size() }, Texture2D{});

        m_Storage.unlock();

        return result.first->second;
    }

    Texture2D Texture2DManager::Get(const std::string_view& name)
    {
        Texture2D res;

        m_Storage.lock();

        if (_storage.contains(name.data()))
        {
            res = _storage.at(std::string{ name.data(), name.size() });
        }

        m_Storage.unlock();

        return res;
    }

    void Texture2DManager::Unload(const std::string_view& name)
    {
        /*
            When we unload a resource, we flag it for removal until the
            las reference to it is destroyed.
        */
        m_Storage.lock();

        if (_storage.contains(name.data()))
        {
            auto& texture = _storage.at(std::string{ name.data(), name.size() });

            m_References.lock();

            auto& reference = _references.at(texture.ID);
            reference.first = true;

            m_References.unlock();

            _storage.erase(std::string{ name.data(), name.size() });
        }

        m_Storage.unlock();
    }

    void Texture2DManager::AddReference(IDType id)
    {
        m_References.lock();

        if (!_references.contains(id))
        {
            _references.insert_or_assign(id, std::make_pair(false, 1u));
        }
        else
        {
            auto& count = _references.at(id);
            ++count.second;
        }

        m_References.unlock();
    }

    size_t Texture2DManager::SubReference(IDType id)
    {
        m_References.lock();

        size_t result = 0u;

        if (_references.contains(id))
        {
            auto& count = _references.at(id);
            --count.second;

            result = count.second;
        }

        m_References.unlock();

        return result;
    }

    ShaderManager::~ShaderManager()
    {
        _references.clear();
        _storage.clear();
    }

    Shader ShaderManager::Load(const std::string_view& name)
    {
        m_Storage.lock();

        if (_storage.contains(name.data()))
        {
            auto& res = _storage.at(std::string{ name.data(), name.size() });

            m_Storage.unlock();

            return Shader{ res };
        }

        auto result = _storage.emplace(std::string{ name.data(), name.size() }, Shader{});

        m_Storage.unlock();

        return result.first->second;
    }

    Shader ShaderManager::Get(const std::string_view& name)
    {
        Shader res;

        m_Storage.lock();

        if (_storage.contains(name.data()))
        {
            res = _storage.at(std::string{ name.data(), name.size() });
        }

        m_Storage.unlock();

        return res;
    }

    void ShaderManager::Unload(const std::string_view& name)
    {
        /*
            When we unload a resource, we flag it for removal until the
            las reference to it is destroyed.
        */
        m_Storage.lock();

        if (_storage.contains(name.data()))
        {
            auto& shader = _storage.at(std::string{ name.data(), name.size() });

            m_References.lock();

            auto& reference = _references.at(shader.ID);
            reference.first = true;

            m_References.unlock();

            _storage.erase(std::string{ name.data(), name.size() });
        }

        m_Storage.unlock();
    }

    void ShaderManager::AddReference(IDType id)
    {
        m_References.lock();

        if (_references.contains(id))
        {
            _references.insert_or_assign(id, std::make_pair(false, 1u));
        }
        else
        {
            auto& count = _references.at(id);
            ++count.second;
        }

        m_References.unlock();
    }

    size_t ShaderManager::SubReference(IDType id)
    {
        m_References.lock();

        size_t result = 0u;

        if (_references.contains(id))
        {
            auto& count = _references.at(id);
            --count.second;

            result = count.second;
        }

        m_References.unlock();

        return result;
    }
}
