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

#include <string>
#include <string_view>
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
	
	
	// Storage maps

	std::unordered_map<const std::string, Shader> _shaders;
	std::unordered_map<const std::string, Texture2D> _textures;

	// Reference counting maps

	std::unordered_map<IDType, uint16_t> _shaderRefCount;
	std::unordered_map<IDType, uint16_t> _textureRefCount;

	/*
		The following private functions must be specialized in the corresponding .cpp file.
	*/

	/*
		Map retrieving functions.
	*/

	template<typename ResType>
	std::unordered_map<const std::string, ResType>& GetStorageMap(const ResType&);

	template<typename ResType>
	std::unordered_map<unsigned int, uint16_t>& GetReferenceMap(const ResType&);


	// Resource Destruction.

	template<typename ResType>
	void DestroyResource(IDType id, ResType temp);

public:
	ResourceManager() = default;
	~ResourceManager();

	/*
		Loading, getting, and unloading resources.
	*/

	template<typename ResType, typename... Args>
	ResType LoadResource(const ResType&, const std::string_view& name, Args&&... args)
	{
		auto& map = GetStorageMap(ResType{});

		if (map.count(name.data()))
		{
			ResType& res = map.at(name.data());

			return ResType{ res };
		}

		auto result = map.insert_or_assign(name.data(), ResType{ std::forward<Args>(args)... });

		return result.first->second;
	}

	template<typename ResType>
	ResType GetResource(const ResType&, const std::string_view& name)
	{
		auto& map = GetStorageMap(ResType{});
		
		if (map.count(name.data()) > 0)
		{
			return map.at(name.data());
		}

		return ResType{};
	}

	template<typename ResType>
	void UnloadResource(const ResType&, const std::string_view& name)
	{
		auto& map = GetStorageMap(ResType{});

		if (map.count(name.data()) > 0)
		{
			map.erase(name.data());
		 }
	}


	/*
		Reference counting functions. Do not touch in client code.
	*/

	template<typename ResType>
	void AddReference(IDType id, const ResType&)
	{
		auto& map = GetReferenceMap(ResType{});

		if (map.count(id) == 0u)
		{
			map.insert_or_assign(id, 1u);
		}
		else
		{
			auto& count = map.at(id);
			++count;
		}
	}

	template<typename ResType>
	void SubReference(IDType id, const ResType&)
	{
		auto& map = GetReferenceMap(ResType{});

		if (map.count(id) > 0u)
		{
			auto& count = map.at(id);
			--count;

			if (count == 0u)
			{
				DestroyResource(id, ResType{});
			}
		}
	}

};

#endif // !PROTOMAPPER_RESOURCE_MANAGER_HPP
