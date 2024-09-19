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

#include <map>
#include <span>

#include "gsl/gsl-lite.hpp"

namespace proto
{
    /*
		This concept is used along side the SharedResource class. Essentially, it
		requires that you provide a structure that allows the template to access the 
		resource's ID as well as a Destroy function for deallocating the resource.

		This Concept is written, primarily, with textures in mind. However, it could be
		adapted to be more generic.
	*/	
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
    
    class RC
	{
	public:

		[[nodiscard]] constexpr auto GetShared(this auto&& self) { return self._shared; }
		[[nodiscard]] constexpr auto GetWeak(this auto&& self) { return self._weak; }
		
		// Increments the owners.
		constexpr void AddShared()
		{
			++_shared;
		}

		/*
			Returns the number of owners remaining.
		*/
		constexpr size_t SubShared()
		{
			--_shared;
			return _shared;
		}

		// Increments the references.
		constexpr void AddWeak()
		{
			++_weak;
		}

		/*
			Returns the number of references remaining.
		*/
		constexpr size_t SubWeak()
		{
			--_weak;
			return _weak;
		}

	private:
		size_t _shared, _weak;
	};

    template<IdentifiedExternal Resource>
    class WeakResource;

    template<IdentifiedExternal Resource>
    class SharedResource
    {
    public:
		constexpr SharedResource() = default;

		friend class SharedResource<Resource>;
		friend class WeakResource<Resource>;

		constexpr SharedResource(const SharedResource& other)
		:  _manager(other._manager), _object(other._object), _empty(false)
		{
			_manager->AddShared();
		}
		
		constexpr SharedResource(const WeakResource<Resource>& ptr)
		:  _manager(ptr._manager), _object(ptr.GetID()), _empty(false)
		{
			_manager->AddShared();
		}

		constexpr SharedResource(SharedResource&& other) noexcept : _manager(other._manager), _object(other._object), _empty(false)
		{
			other.Nullify();
		}

		constexpr SharedResource& operator=(SharedResource&& other) noexcept
		{
			if (!_empty && _manager->SubShared() == 0z)
			{
				_object.Destroy();

				if(_manager->GetWeak() == 0z)
				{
					delete _manager;
				}
			}

			_empty = false;
			_object = other.Get();
			_manager = gsl::owner<RC*>{other._manager};
			_manager->AddShared();

			other.Nullify();

			return *this;
		}

		template<typename... Args>
		constexpr SharedResource(Args&&... args)
		: _manager(gsl::owner<RC*>{new RC{}}), _object(std::forward<Args>(args)...), _empty(false)
		{
			_manager->AddShared();
		}

		constexpr ~SharedResource()
		{
			if (!_empty && _manager->SubShared() == 0z)
			{
				_object.Destroy();

				if(_manager->GetWeak() == 0z)
				{
					delete _manager;
				}
			}
		}

		constexpr SharedResource& operator=(const SharedResource& other)
		{
			if (!_empty && _manager->SubShared() == 0z)
			{
				_object.Destroy();
				
				if(_manager->GetWeak() == 0z)
				{
					delete _manager;
				}
			}

			_empty = false;
			_object = other.Get();
			_manager = gsl::owner<RC*>{other._manager};
			_manager->AddShared();

			return *this;
		}

		constexpr bool operator==(const SharedResource& rhs) const { return (_object == rhs.Get()); }

		[[nodiscard]] constexpr auto Get(this auto&& self) { return self._object; }
		[[nodiscard]] constexpr auto GetID(this auto&& self) { return self._object.GetID(); }
		[[nodiscard]] constexpr auto GetOwners(this auto&& self) { return (!self._empty) ? self._manager->GetShared() : 0z; }

    private:
		constexpr void Nullify(this auto&& self)
		{
			self._manager = nullptr;
			self._empty = true;
		}

		gsl::owner<RC*> _manager = nullptr;
		Resource _object;
		bool _empty = true;
    };
    
    template<IdentifiedExternal Resource, typename... Args>
    [[nodiscard]] constexpr auto MakeResource(Args&&... args)
    {
		return SharedResource<Resource>{ std::forward<Args>(args)... };
    }

    template<IdentifiedExternal Resource>
    [[nodiscard]] constexpr auto MakeResource(const SharedResource<Resource>& other)
    {
		return SharedResource<Resource>{ other };
    }

    template<IdentifiedExternal Resource>
    [[nodiscard]] constexpr auto MakeResource()
    {
		return SharedResource<Resource>{ Resource{} };
    }

    template<IdentifiedExternal Resource>
    class WeakResource
    {
		friend SharedResource<Resource>;
		friend WeakResource<Resource>;
    public:		
		constexpr WeakResource(const SharedResource<Resource>& owner)
		: _manager(owner._manager), ID(owner.ID)
		{
			_manager->AddWeak();
		}

		constexpr WeakResource(const WeakResource& other)
		: _manager(other._manager), ID(other.ID)
		{
			_manager->AddWeak();
		}

		constexpr WeakResource& operator=(const WeakResource& other)
		{
			if(_manager->SubWeak() == 0z && _manager->GetShared() == 0z)
			{
				delete _manager;
			}

			_manager = gsl::owner<RC*>{other._manager};
			ID = other.ID;
			_manager->AddWeak();

			return *this;
		}

		WeakResource(WeakResource&&) = delete;
		WeakResource& operator=(WeakResource&&) = delete;

		constexpr bool operator==(const WeakResource& other) const { return ID == other.ID; }

		constexpr ~WeakResource()
		{
			if(_manager->SubWeak() == 0z && _manager->GetShared() == 0z)
			{
				delete _manager;
			}
		}
    
		[[nodiscard]] constexpr auto GetOwners(this auto&& self) { return self._manager->GetShared(); }
		[[nodiscard]] constexpr bool Expired(this auto&& self) { return self.GetOwners() == 0z; }
		[[nodiscard]] constexpr auto&& Lock(this auto&& self) { return (!self.Expired()) ? SharedResource{std::forward<WeakResource<Resource>>(self)} : SharedResource<Resource>{}; }

    private:
		gsl::owner<RC*> _manager = nullptr;
		Resource::IDType ID;
    };

}

#endif
