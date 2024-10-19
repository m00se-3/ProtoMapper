#ifndef PROTO_DYN_ARRAY_HPP
#define PROTO_DYN_ARRAY_HPP

#include <gsl/gsl-lite.hpp>
#include <stdexcept>

namespace proto
{

    template <typename T>
    class dyn_array
    {
    public:
        friend class dyn_array<T>;

        class iterator 
        {
        public:
            friend class iterator;
            constexpr explicit iterator(T* ptr) : _location(ptr) {}
            constexpr T& get(this auto&& self) { return *self._location; }

            constexpr iterator& operator++() // pre-increment
            {
                ++_location;
                return *this;
            }

            constexpr iterator operator++(int) // post-increment
            {
                auto temp = _location;
                ++_location;
                return iterator{temp};
            }

            constexpr iterator& operator--() // pre-decrement
            {
                --_location;
                return *this;
            }

            constexpr iterator operator--(int) // post-decrement
            {
                auto temp = _location;
                --_location;
                return iterator{temp};
            }

            constexpr bool operator==(const iterator& other) const { return _location == other._location; }
            constexpr auto operator<=>(const iterator&) const = default;

        private:
            T* _location = nullptr;
        };

        class const_iterator 
        {
        public:
            friend class const_iterator;
            constexpr explicit const_iterator(T* ptr) : _location(ptr) {}
            constexpr const T& get(this auto&& self) { return *self._location; }

            constexpr const_iterator& operator++() // pre-increment
            {
                ++_location;
                return *this;
            }

            constexpr const_iterator operator++(int) // post-increment
            {
                auto temp = _location;
                ++_location;
                return iterator{temp};
            }

            constexpr const_iterator& operator--() // pre-decrement
            {
                --_location;
                return *this;
            }

            constexpr const_iterator operator--(int) // post-decrement
            {
                auto temp = _location;
                --_location;
                return iterator{temp};
            }

            constexpr bool operator==(const const_iterator& other) const { return _location == other._location; }
            constexpr auto operator<=>(const const_iterator&) const = default;

        private:
            const T* _location = nullptr;
        };

        constexpr explicit dyn_array(size_t capacity)
        : _data(::operator new(capacity)), _capacity(capacity)
        {
        }

        constexpr dyn_array(size_t capacity, const auto& initVal)
        : _data(new T[capacity](initVal)), _size(capacity), _capacity(capacity)
        {
        }

        constexpr dyn_array(const dyn_array& other)
        : _data(::operator new(other._capacity)), _size(other._size), _capacity(other._capacity)
        {
            auto index = 0uz;
            
            for (auto& item : other)
            {
                _data[index] = T{item};
                ++index;
            }
        }

        constexpr dyn_array(dyn_array&& other) noexcept
        : _data(other._data), _size(other._size), _capacity(other._capacity)
        {
            other.nullify();
        }

        [[nodiscard]] constexpr dyn_array& operator=(const dyn_array& other)
        {
            if(this == &other) { return *this; }
        }

        [[nodiscard]] constexpr dyn_array& operator=(dyn_array&& other) noexcept
        {
            if(this == &other) { return *this; }

            delete[] _data;
            _data = gsl::owner<T*>{other._data};
            _size = other._size;
            _capacity = other._capacity;

            other.nullify();
        }

        constexpr ~dyn_array()
        {
            clear();
            ::operator delete(_data);
        }

        constexpr T& operator[](size_t index) { return _data[index]; }

        constexpr auto& at(this auto&& self, size_t index)
        {
            if(index < self._size)
            {
                return self._data[index];
            }

            throw std::out_of_range("Index out of range.");
        }

        template <typename... Args>
        constexpr T& emplace_back(this auto&& self, Args&&... args)
        {
            if(self._size == self._capacity) { throw std::overflow_error("Unable to fit more items."); }

            ++self._size;
            self._data[self._size] = T{std::forward<Args>(args)...};

            return self.back();
        }

        template <typename...Args>
        constexpr T& push_back(this auto&& self, Args&&... args)
        {
            return self.emplace_back(std::forward<Args>(args)...);
        }

        constexpr T& push_back(this auto&& self, const T& item) { return self.emplace_back(item); }

        constexpr iterator begin(this auto&& self) { return self._data; }
        constexpr iterator end(this auto&& self) { return self._data + self._size; }
        constexpr const_iterator cbegin(this auto&& self) { return self._data; }
        constexpr const_iterator cend(this auto&& self) { return self._data + self._size; }

        constexpr T& front(this auto&& self) 
        {
            if(self._size == 0uz) { throw std::out_of_range("dyn_array contains no items."); }
            return self._data[0uz]; 
        }
        constexpr T& back(this auto&& self) 
        {
            if(self._size == 0uz) { throw std::out_of_range("dyn_array contains no items."); }
            return self._data[self._size - 1uz];
        }

        constexpr bool empty(this auto&& self) { return self._size == 0uz; }

        constexpr void clear()
        {
            auto start = _data;
            const auto end = _data + _size;

            while(start != end)
            {
                start.~T();
                ++start;
            }

            _size = 0uz;
        }

    private:
        constexpr void nullify()
        {
            _data = gsl::owner<T*>{nullptr};
            _size = 0uz;
            _capacity = 0uz;
        }

        gsl::owner<T*> _data = nullptr;
        size_t _size{}, _capacity{};
    };
}

#endif