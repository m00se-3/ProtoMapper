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
#ifndef PROTO_RESOURCE_MANAGER_HPP
#define PROTO_RESOURCE_MANAGER_HPP

#include <memory_resource>
#include <map>
#include <span>

#include "gsl/gsl-lite.hpp"

namespace proto
{
    template<typename T>
    concept IdentifiedExternal = requires(T t, T o)
    {
	    std::default_initializable<T>;
	    std::copy_constructible<T>;
	    std::copyable<T>;
	    { t == o } -> std::same_as<bool>;
	    { t.GetID() } -> std::unsigned_integral;
	    { t.Destroy() } -> std::same_as<void>;
    };
    
    class RC;


    /*
	The ResouceManager is in charge of storing shaders, textures, and other external data resources.
	It also keeps a reference count of shaders and textures.
    */
    class ResourceManager
    {
    public:
	ResourceManager(const std::span<uint8_t>& resource);

	void LoadStringRes(const std::string& name, const std::string& content);
	std::string_view GetString(const std::string_view& name);
	void UnloadString(const std::string_view& name);

    private:
	std::pmr::monotonic_buffer_resource _upstream;
	std::pmr::synchronized_pool_resource _textAllocator;

	// String storage map.
	std::pmr::map<std::pmr::string, std::pmr::string> _stringMap;
    };

    template<IdentifiedExternal Resource>
    class WeakResource;

    class RC
    {
    public:

	[[nodiscard]] auto GetCount(this auto&& self) { return self._count; }
	
	// Increments the reference.
	constexpr void Add()
	{
	    ++_count;
	}

	/*
	    Returns the number of references remaining.
	*/
	constexpr size_t Sub()
	{
	    --_count;
	    return _count;
	}

    private:
	size_t _count;
    };

    template<IdentifiedExternal Resource>
    class SharedResource
    {
    public:
	SharedResource() = default;

	SharedResource(const SharedResource& other)
	: _object(other.Get()), _manager(other.GetRC())
	{
	    _manager->Add();
	}
	
	SharedResource(WeakResource<Resource>&& ptr)
	: _object(ptr.GetID()), _manager(ptr.GetReferenceCounter())
	{}

	SharedResource(SharedResource&& other) noexcept : _object(other.Get()), _manager(other.GetRC()) {}

	SharedResource& operator=(SharedResource&& other) noexcept
	{
	    if (_manager->Sub() == 0u)
	    {
		_object.Destroy();
		delete _manager;
	    }

	    _object = other.Get();
	    _manager = gsl::owner<RC*>{other.GetRC()};
	    _manager->Add();

	    return *this;
	}

	template<typename... Args>
	SharedResource(Args&&... args)
	: _object(std::forward<Args>(args)...), _manager(gsl::owner<RC*>{new RC{}})
	{
	    _manager->Add();
	}

	~SharedResource()
	{
	    if (_manager->Sub() == 0u)
	    {
		_object.Destroy();
		delete _manager;
	    }
	}

	SharedResource& operator=(const SharedResource& other)
	{
	    if (_manager->Sub() == 0u)
	    {
		_object.Destroy();
		delete _manager;
	    }

	    _object = other.Get();
	    _manager = gsl::owner<RC*>{other.GetRC()};
	    _manager->Add();

	    return *this;
	}

	bool operator==(const SharedResource& rhs) { return (_object == rhs.Get()); }
	gsl::owner<RC*> GetRC() { return _manager; }

	[[nodiscard]] auto Get(this auto&& self) { return self._object; }
	[[nodiscard]] auto GetID(this auto&& self) { return self._object.GetID(); }

    private:
	Resource _object;
	gsl::owner<RC*> _manager;
    };
    
    template<IdentifiedExternal Resource, typename... Args>
    [[nodiscard]] constexpr auto MakeResource(Args&&... args)
    {
	return SharedResource<Resource>{ std::forward<Args>(args)... };
    }

    template<IdentifiedExternal Resource>
    [[nodiscard]] constexpr auto MakeResource(SharedResource<Resource>&& other)
    {
	return SharedResource<Resource>{ other };
    }

    template<IdentifiedExternal Resource>
    [[nodiscard]] constexpr auto MakeResource()
    {
	return SharedResource<Resource>{ };
    }

    template<IdentifiedExternal Resource>
    class WeakResource
    {
    public:
	WeakResource(const SharedResource<Resource>& owner)
	: _manager(owner.GetRC()), ID(owner.GetID())
	{}
    
	[[nodiscard]] bool Valid(this auto&& self) { return self._manager->GetCount(); }
	[[nodiscard]] auto Lock(this auto&& self) { return SharedResource{std::forward<WeakResource<Resource>>(self)}; }
	[[nodiscard]] auto GetID(this auto&& self) { return self.ID; }

    private:
	RC* _manager;
	Resource::IDType ID;
    };

}

#endif
