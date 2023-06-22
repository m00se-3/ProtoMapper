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
#include <optional>

/*
	The ResouceManager is in charge of storing shaders, textures, and other external data resources.
	It also keeps a reference count of each resource.

	The class uses a template structure for modifying the resource containers that I haven't tried before.
	The purpose is to try and minimize the number of functions that must be added per resource container.
*/
class ResourceManager
{
	using IDType = unsigned int;
	
	std::pmr::monotonic_buffer_resource _textResource;
	std::pmr::synchronized_pool_resource _textAllocator;
	
	// Storage maps

	std::unordered_map<std::string, Shader> _shaders;
	std::unordered_map<std::string, Texture2D> _textures;
	std::pmr::map<std::pmr::string, std::pmr::string> _stringMap;

	// Reference counting maps

	std::unordered_map<IDType, std::pair<bool, uint16_t>> _shaderRefCount;
	std::unordered_map<IDType, std::pair<bool, uint16_t>> _textureRefCount;


public:
	ResourceManager(void* memory, size_t size);
	~ResourceManager();

	// Strings are handled internally, so using the reference counting templates doesn't make sense.

	std::string_view LoadString(const std::string& name, const std::string& content);
	std::string_view GetString(const std::string_view& name);
	void UnloadString(const std::string_view& name);

	/*
		Loading, getting, and unloading reference counted resources.
	*/

	Texture2D LoadTexture(const std::string_view& name);
	Texture2D GetTexture(const std::string_view& name);
	void UnloadTexture(const std::string_view& name);

	Shader LoadShader(const std::string_view& name);
	Shader GetShader(const std::string_view& name);
	void UnloadShader(const std::string_view& name);

	/*
		Reference counting functions.
	*/

	void AddRefTexture(IDType id);
	void SubRefTexture(IDType id);
	void AddRefShader(IDType id);
	void SubRefShader(IDType id);
};

#endif // !PROTOMAPPER_RESOURCE_MANAGER_HPP
