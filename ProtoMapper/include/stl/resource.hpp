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
#ifndef PROTOSTL_RESOURCE_HPP
#define PROTOSTL_RESOURCE_HPP

#include <gsl/gsl-lite.hpp>
#include <concepts>
#include <functional>

namespace proto
{
	inline constexpr auto shared_res_default_destructor = [](auto&) {};

	template<typename Resource>
    class RC
	{
	public:
		template<typename... Args>
		RC(const std::invocable<Resource&> auto& dest, Args&&... args): _object(new Resource{std::forward<Args>(args)...}), _destructor(std::move(dest)) {}

		[[nodiscard]] constexpr auto get_shared(this auto&& self) { return self._shared; }
		[[nodiscard]] constexpr auto get_weak(this auto&& self) { return self._weak; }
		[[nodiscard]] constexpr auto& get(this auto&& self) { return *(self._object); }
		[[nodiscard]] constexpr auto get_ptr(this auto&& self) { return self._object; }
		
		// Increments the owners.
		constexpr void add_shared()
		{
			++_shared;
		}

		/*
			Returns the number of owners remaining.
		*/
		[[nodiscard]] constexpr size_t sub_shared()
		{
			--_shared;
			return _shared;
		}

		// Increments the references.
		constexpr void add_weak()
		{
			++_weak;
		}

		/*
			Returns the number of references remaining.
		*/
		[[nodiscard]] constexpr size_t sub_weak()
		{
			--_weak;
			return _weak;
		}

		constexpr void destroy_object()
		{
			std::invoke(_destructor, std::forward<Resource&>(*_object));
			delete _object;
		}

	private:

		uint32_t _shared{}, _weak{};
		Resource* _object = nullptr;
		std::function<void(Resource&)> _destructor;
	};

    template<typename Resource>
    class weak_res;

    template<typename Resource>
    class shared_res
    {
    public:
		constexpr shared_res() = default;

		friend class shared_res<Resource>;
		friend class weak_res<Resource>;

		constexpr shared_res(const shared_res& other)
		:  _manager(gsl::owner<RC<Resource>*>{other._manager}), _object(other.get_ptr())
		{
			_manager->add_shared();
		}

		constexpr shared_res(shared_res& other)
		:  _manager(gsl::owner<RC<Resource>*>{other._manager}), _object(other.get_ptr())
		{
			_manager->add_shared();
		}
		
		constexpr shared_res(const weak_res<Resource>& ptr)
		:  _manager(gsl::owner<RC<Resource>*>{ptr._manager}), _object(ptr.get_ptr())
		{
			_manager->add_shared();
		}

		constexpr shared_res(shared_res&& other) noexcept : _manager(gsl::owner<RC<Resource>*>{other._manager}), _object(other._object)
		{
			other.nullify();
		}

		constexpr shared_res& operator=(shared_res&& other) noexcept
		{
			reset();

			_object = other.get_ptr();
			_manager = gsl::owner<RC<Resource>*>{other._manager};
			_manager->add_shared();

			other.nullify();

			return *this;
		}

		constexpr shared_res& operator=(const shared_res& other)
		{
			if(this == &other) { return *this; }
			
			reset();

			_object = other.get_ptr();
			_manager = gsl::owner<RC<Resource>*>{other._manager};
			_manager->add_shared();

			return *this;
		}

		template<typename... Args>
		constexpr shared_res(const std::invocable<Resource&> auto& dest, Args&&... args)
		: _manager(gsl::owner<RC<Resource>*>{new RC<Resource>{dest, std::forward<Args>(args)...}}), _object(_manager->get_ptr())
		{
			_manager->add_shared();
		}

		constexpr ~shared_res()
		{
			reset();
		}

		constexpr void reset()
		{
			if (_object != nullptr && _manager->sub_shared() == 0z)
			{
				destroy_object();

				if(_manager->get_weak() == 0z)
				{
					delete _manager;
				}
			}

			nullify();
		}

		constexpr void reset(const shared_res& other)
		{
			reset();

			_object = other.get_ptr();
			_manager = gsl::owner<RC<Resource>*>{other._manager};
			_manager->add_shared();
		}

		constexpr bool operator==(const shared_res& rhs) const { return (_object == rhs.get_ptr()); }

		[[nodiscard]] constexpr auto& get(this auto&& self) { return *self._object; }
		[[nodiscard]] constexpr auto get_ptr(this auto&& self) { return self._object; }
		[[nodiscard]] constexpr auto get_owners(this auto&& self) { return (self._manager != nullptr && self._object != nullptr) ? self._manager->get_shared() : 0z; }
		[[nodiscard]] constexpr auto get_refs(this auto&& self) { return (self._manager != nullptr && self._object != nullptr) ? self._manager->get_refs() : 0z; }

    private:
		constexpr void nullify(this auto&& self)
		{
			self._manager = nullptr;
			self._object = nullptr;
		}

		constexpr void destroy_object(this auto&& self)
		{
			self._manager->destroy_object();
			self._object = nullptr;
		}

		gsl::owner<RC<Resource>*> _manager = nullptr;
		Resource* _object;
    };
    
    template<typename Resource, typename... Args>
    [[nodiscard]] constexpr auto make_shared_res(const std::invocable<Resource&> auto& dest, Args&&... args)
    {
		return shared_res<Resource>{ dest, std::forward<Args>(args)... };
    }

    template<typename Resource>
    [[nodiscard]] constexpr auto make_shared_res(const shared_res<Resource>& other)
    {
		return shared_res<Resource>{ other };
    }

    template<typename Resource>
    [[nodiscard]] constexpr auto make_shared_res()
    {
		return shared_res<Resource>{ Resource{} };
    }

    template<typename Resource>
    class weak_res
    {
		friend shared_res<Resource>;
		friend weak_res<Resource>;
    public:		
		constexpr weak_res(const shared_res<Resource>& owner)
		: _manager(owner._manager)
		{
			_manager->add_weak();
		}

		constexpr weak_res(const weak_res& other)
		: _manager(other._manager)
		{
			_manager->add_weak();
		}

		constexpr weak_res& operator=(const weak_res& other)
		{
			if(this == &other) { return *this; }
			
			if(_manager->sub_weak() == 0z && _manager->get_shared() == 0z)
			{
				delete _manager;
			}

			_manager = gsl::owner<RC<Resource>*>{other._manager};
			_manager->add_weak();

			return *this;
		}

		weak_res(weak_res&&) = delete;
		weak_res& operator=(weak_res&&) = delete;

		constexpr bool operator==(const weak_res& other) const { return _manager == other._manager; }

		constexpr ~weak_res()
		{
			if(_manager->sub_weak() == 0z && _manager->get_shared() == 0z)
			{
				delete _manager;
			}
		}
    
		[[nodiscard]] constexpr auto get_owners(this auto&& self) { return self._manager->get_shared(); }
		[[nodiscard]] constexpr auto get_refs(this auto&& self) { return self._manager->get_weak(); }
		[[nodiscard]] constexpr bool expired(this auto&& self) { return self.get_owners() == 0z; }
		[[nodiscard]] constexpr auto&& lock(this auto&& self) { return (!self.expired()) ? shared_res{std::forward<weak_res<Resource>>(self)} : shared_res<Resource>{}; }

    private:
		gsl::owner<RC<Resource>*> _manager = nullptr;
    };

}

#endif
