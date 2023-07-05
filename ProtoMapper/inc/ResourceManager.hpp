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

#ifndef PROTOMAPPER_RESOURCE_MANAGER_HPP
#define PROTOMAPPER_RESOURCE_MANAGER_HPP

#include "Texture.hpp"
#include "Shader.hpp"

#include <memory_resource>
#include <string>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <map>
#include <mutex>

class Texture2DManager;
class ShaderManager;

template<typename Key, typename Value>
struct StorageMap
{
	std::unordered_map<Key, Value> map;

	Value Load(const std::string_view& name);
	Value Get(const std::string_view& name);
	void Unload(const std::string_view& name);
};

template<typename IDType>
struct ReferenceMap
{
	std::unordered_map<IDType, std::pair<bool, uint16_t>> _references;

	void AddReference(IDType id);
	void SubReference(IDType id);
};

/*
	The ResouceManager is in charge of storing shaders, textures, and other external data resources.
	It also keeps a reference count of each resource.

	The class uses a template structure for modifying the resource containers that I haven't tried before.
	The purpose is to try and minimize the number of functions that must be added per resource container.
*/
class ResourceManager
{
	std::pmr::monotonic_buffer_resource _textResource;
	std::pmr::synchronized_pool_resource _textAllocator;

	std::mutex _mutex;
	
	// String storage map.

	std::pmr::map<std::pmr::string, std::pmr::string> _stringMap;

	// Separate handlers for textures and shaders allows for easier data sharing between threads.

	std::unique_ptr<Texture2DManager> _textures;
	std::unique_ptr<ShaderManager> _shaders;


public:
	ResourceManager(void* memory, size_t size);
	~ResourceManager();

	// Mutex operations.

	void Lock();
	void Unlock();

	/*
		Functions for assigning data references to the appropriate parts.
	*/
	
	Texture2DManager* Textures();
	ShaderManager* Shaders();

	// String handling functions.

	std::string_view LoadString(const std::string& name, const std::string& content);
	std::string_view GetString(const std::string_view& name);
	void UnloadString(const std::string_view& name);

};

class Texture2DManager
{
	using IDType = unsigned int;

	std::mutex m_Storage, m_References;

	std::unordered_map<std::string, Texture2D> _storage;
	std::unordered_map<IDType, std::pair<bool, uint16_t>> _references;

public:
	Texture2DManager() = default;
	Texture2DManager(const Texture2DManager&) = delete;
	Texture2DManager(Texture2DManager&&) = delete;
	~Texture2DManager();

	Texture2D Load(const std::string_view& name);
	Texture2D Get(const std::string_view& name);
	void Unload(const std::string_view& name);

	/*
		Reference counting.
	*/

	// Increments the reference count for the resource identified by id.
	void AddReference(IDType id);

	/*
		Decrements the reference count of the resource identified by id.

		Returns the number of references remaining.
	*/
	size_t SubReference(IDType id);
};

class ShaderManager
{
	using IDType = unsigned int;

	std::mutex m_Storage, m_References;

	std::unordered_map<std::string, Shader> _storage;
	std::unordered_map<IDType, std::pair<bool, uint16_t>> _references;

public:
	ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	ShaderManager(ShaderManager&&) = delete;
	~ShaderManager();

	Shader Load(const std::string_view& name);
	Shader Get(const std::string_view& name);
	void Unload(const std::string_view& name);

	/*
		Reference counting.
	*/

	// Increments the reference count for the resource identified by id.
	void AddReference(IDType id);

	/*
		Decrements the reference count of the resource identified by id.

		Returns the number of references remaining.
	*/
	size_t SubReference(IDType id);
};

#endif // !PROTOMAPPER_RESOURCE_MANAGER_HPP
