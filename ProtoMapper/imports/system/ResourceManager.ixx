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
#include <map>
#include <unordered_map>
#include <span>
#include <functional>

export module proto.ResourceManager;

import proto.Texture;
import proto.Shader;

namespace proto
{
    export template<typename T>
	concept IdentifiedExternal = requires(T t, T o)
	{
		std::default_initializable<T>;
		std::copy_constructible<T>;
		std::copyable<T>;
		{ t == o } -> std::same_as<bool>;
		{ t.Valid() } -> std::same_as<bool>;
		{ t.GetID() } -> std::unsigned_integral;
		{ t.Destroy() } -> std::same_as<void>;
	};
	
	template<IdentifiedExternal Resource>
	class ReferenceCounter;


	/*
		The ResouceManager is in charge of storing shaders, textures, and other external data resources.
		It also keeps a reference count of shaders and textures.
	*/
	export class ResourceManager
	{
	public:
		ResourceManager(const std::span<uint8_t>& resource);
		~ResourceManager() = default;
		
		ReferenceCounter<Texture2D>* Textures();
		ReferenceCounter<Shader>* Shaders();

		void LoadStringRes(const std::string& name, const std::string& content);
		std::string_view GetString(const std::string_view& name);
		void UnloadString(const std::string_view& name);

	private:
		std::pmr::monotonic_buffer_resource _upstream;
		std::pmr::synchronized_pool_resource _textAllocator;

		// String storage map.
		std::pmr::map<std::pmr::string, std::pmr::string> _stringMap;

		std::unique_ptr<ReferenceCounter<Texture2D>> _textures;
		std::unique_ptr<ReferenceCounter<Shader>> _shaders;

	};

	export template<IdentifiedExternal Resource>
	class GPUResource
	{
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

	private:
		Resource _object;

		static ReferenceCounter<Resource>* _manager;
	};

	export template<IdentifiedExternal Resource>
	class ReferenceCounter
	{
	public:
		using IDType = unsigned int;

		ReferenceCounter() = default;
		ReferenceCounter(const ReferenceCounter&) = delete;
		ReferenceCounter(ReferenceCounter&&) = delete;

		~ReferenceCounter()
		{
			_references.clear();
			_storage.clear();
		}

		GPUResource<Resource> Load(const std::string_view& name, const std::function<void(Resource&)>& onLoad = [](Resource&){})
		{
			if (_storage.contains(name.data()))
			{
				auto& res = _storage.at(std::string{ name.data(), name.size() });

				return res;
			}

			_storage.emplace(std::string{ name.data(), name.size() }, Resource{});
			auto& result = _storage.at(std::string{ name.data(), name.size() });

			onLoad(result.Get());

			return result;
		}

		void LoadCopy(const std::string_view& name, Resource& cpy)
		{
			if (!_storage.contains(name.data()))
			{
				auto result = _storage.emplace(std::string{ name.data(), name.size() }, cpy);

				AddReference(result.first->second.Get().GetID());		// Make sure our reference count is correct, since we are copying.
			}
		}

		GPUResource<Resource> Get(const std::string_view& name)
		{
			if (_storage.contains(name.data()))
			{
				return _storage.at(std::string{ name.data(), name.size() });
			}

			return GPUResource<Resource>{};
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

	private:
		std::unordered_map<std::string, GPUResource<Resource>> _storage;
		std::unordered_map<IDType, std::pair<bool, uint16_t>> _references;
	};
    
}