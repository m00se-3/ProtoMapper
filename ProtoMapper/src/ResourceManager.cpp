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
module;

#include <memory_resource>
#include <span>
#include <string>
#include <string_view>
#include <memory>
#include <unordered_map>
#include <map>

export module ResourceManager;

import Concepts;
export import Texture;
export import Shader;
export namespace proto
{
    template<IdentifiedExternal Resource>
	class ReferenceCounter;

    	template<IdentifiedExternal Resource>
	class GPUResource
	{
		Resource _object;

		static ReferenceCounter<Resource>* _manager;

	public:
		GPUResource()
			: _object()
		{
			_manager->AddReference(_object.GetID());
		}

		GPUResource(GPUResource& other)
			: _object(other.Get())
		{
			_manager->AddReference(_object.GetID());
		}

		template<typename... Args>
		GPUResource(Args&&... args)
			: _object(std::forward<Args>(args)...)
		{
			_manager->AddReference(_object.GetID());
		}

		~GPUResource()
		{
			if (_manager->SubReference(_object.GetID()) == 0u) _object.Destroy();
		}

		GPUResource& operator=(const GPUResource& other)
		{
			if (!_object.Valid() && !_manager->SubReference(_object.GetID())) _object.Destroy();

			_object = other.Get();
			_manager->AddReference(_object.GetID());

			return *this;
		}

		bool operator==(const GPUResource& rhs) { return (_object == rhs.Get()); }

		Resource& Get() { return _object; }

		const Resource& Get() const { return _object; }

		static void SetManager(ReferenceCounter<Resource>* ptr) { _manager = ptr; }
	};

	template<IdentifiedExternal Resource>
	class ReferenceCounter
	{
		using IDType = unsigned int;

		std::unordered_map<std::string, GPUResource<Resource>> _storage;
		std::unordered_map<IDType, std::pair<bool, uint16_t>> _references;

	public:
		ReferenceCounter() = default;
		ReferenceCounter(const ReferenceCounter&) = delete;
		ReferenceCounter(ReferenceCounter&&) = delete;

		~ReferenceCounter()
		{
			_references.clear();
			_storage.clear();
		}

		GPUResource<Resource> Load(const std::string_view& name)
		{
			if (_storage.contains(name.data()))
			{
				auto& res = _storage.at(std::string{ name.data(), name.size() });

				return res;
			}

			auto result = _storage.emplace(std::string{ name.data(), name.size() }, Resource{});

			return result.first->second;
		}

		GPUResource<Resource> Get(const std::string_view& name)
		{
			GPUResource<Resource> res;

			if (_storage.contains(name.data()))
			{
				res = _storage.at(std::string{ name.data(), name.size() });
			}

			return res;
		}

		void Unload(const std::string_view& name)
		{
			/*
				When we unload a resource, we flag it for removal until the
				last reference to it is destroyed.
			*/
			if (_storage.contains(name.data()))
			{
				auto& res = _storage.at(std::string{ name.data(), name.size() });

				auto& reference = _references.at(res.Get().GetID());
				reference.first = true;

				_storage.erase(std::string{ name.data(), name.size() });
			}

		}

		/*
			Reference counting.
		*/

		// Increments the reference count for the resource identified by id.
		void AddReference(IDType id)
		{
			if (!_references.contains(id))
			{
				_references.insert_or_assign(id, std::make_pair(false, 1u));
			}
			else
			{
				auto& count = _references.at(id);
				++count.second;
			}
		}

		/*
			Decrements the reference count of the resource identified by id.

			Returns the number of references remaining.
		*/
		size_t SubReference(IDType id)
		{
			size_t result = 0u;

			if (_references.contains(id))
			{
				auto& count = _references.at(id);
				--count.second;

				result = count.second;
			}

			return result;
		}
	};
    
    template<>
    ReferenceCounter<Shader>* GPUResource<Shader>::_manager = nullptr;

    template<>
    ReferenceCounter<Texture2D>* GPUResource<Texture2D>::_manager = nullptr;


	/*
		The ResouceManager is in charge of storing shaders, textures, and other external data resources.
		It also keeps a reference count of each resource.
	*/
	export class ResourceManager
	{
		
		std::pmr::monotonic_buffer_resource _upstream;
		std::pmr::synchronized_pool_resource _textAllocator;

		// String storage map.
		std::pmr::map<std::pmr::string, std::pmr::string> _stringMap;

		std::unique_ptr<ReferenceCounter<Texture2D>> _textures;
		std::unique_ptr<ReferenceCounter<Shader>> _shaders;


	public:
		ResourceManager(const std::span<uint8_t>& resource)
            : _upstream(resource.data(), resource.size()), _textAllocator(&_upstream), 
            _textures(std::make_unique<ReferenceCounter<Texture2D>>()), _shaders(std::make_unique<ReferenceCounter<Shader>>())
        {
            GPUResource<Texture2D>::SetManager(_textures.get());
            GPUResource<Shader>::SetManager(_shaders.get());
        }

		~ResourceManager() = default;
		
		ReferenceCounter<Texture2D>* Textures() { return _textures.get(); }
		ReferenceCounter<Shader>* Shaders() { return _shaders.get(); }

		void LoadStringRes(const std::string& name, const std::string& content)
        {
            auto str = _stringMap.emplace(
                std::pmr::string{ name, &_textAllocator },
                std::pmr::string{ content, &_textAllocator }
            );
        }

		std::string_view GetString(const std::string_view& name)
        {
            if (_stringMap.contains(name.data()))
            {
                auto& result = _stringMap.at(std::pmr::string{ name.data(), name.size(), &_textAllocator });

                std::string_view view{ result.c_str(), result.size() };

                return view;
            }

            return std::string_view{};
        }

		void UnloadString(const std::string_view& name)
        {
            _stringMap.erase(std::pmr::string{ name.data(), name.size(), &_textAllocator });
        }

	};

}
