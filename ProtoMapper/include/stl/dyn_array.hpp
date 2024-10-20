#ifndef PROTO_DYN_ARRAY_HPP
#define PROTO_DYN_ARRAY_HPP

#include <iterator>
#include <stdexcept>
#include <span>

namespace proto
{
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    /**
     * @brief A contiguous container whose capacity is allocated once, at construction, and then never reallocates.
     * 
     * @tparam T The type of the data contained.
     */
    template <typename T>
    class dyn_array
    {
    public:
        friend class dyn_array<T>;

        class iterator 
        {
        public:
            friend class iterator;

            using difference_type = std::iter_difference_t<T>;
            using value_type = T;

            constexpr iterator() = default;
            constexpr explicit iterator(T* ptr) : _it(ptr) {}
            constexpr T* get(this auto&& self) { return self._it; }

            constexpr iterator& operator++() // pre-increment
            {
                ++_it;
                return *this;
            }

            constexpr iterator operator++(int) // post-increment NOLINT
            {
                auto temp = _it;
                ++_it;
                return iterator{temp};
            }

            constexpr iterator& operator--() // pre-decrement
            {
                --_it;
                return *this;
            }

            constexpr iterator operator--(int) // post-decrement NOLINT
            {
                auto temp = _it;
                --_it;
                return iterator{temp};
            }

            constexpr T& operator*(this auto&& self) { return *self._it; }
            constexpr bool operator==(const iterator& other) const { return _it == other._it; }
            constexpr auto operator<=>(const iterator&) const = default;

        private:
            T* _it = nullptr;
        };

        class const_iterator 
        {
        public:
            friend class const_iterator;

            using difference_type = std::iter_difference_t<T>;
            using value_type = T;

            constexpr const_iterator() = default;
            constexpr explicit const_iterator(T* ptr) : _it(ptr) {}
            constexpr const T* get(this auto&& self) { return self._it; }

            constexpr const_iterator& operator++() // pre-increment
            {
                ++_it;
                return *this;
            }

            constexpr const_iterator operator++(int) // post-increment NOLINT
            {
                auto temp = _it;
                ++_it;
                return const_iterator{temp};
            }

            constexpr const_iterator& operator--() // pre-decrement
            {
                --_it;
                return *this;
            }

            constexpr const_iterator operator--(int) // post-decrement NOLINT
            {
                auto temp = _it;
                --_it;
                return const_iterator{temp};
            }

            constexpr const T& operator*(this auto&& self) { return *self._it; }
            constexpr bool operator==(const const_iterator& other) const { return _it == other._it; }
            constexpr auto operator<=>(const const_iterator&) const = default;

        private:
            const T* _it = nullptr;
        };

        class reverse_iterator 
        {
        public:
            friend class reverse_iterator;
            
            using difference_type = std::iter_difference_t<T>;
            using value_type = T;

            constexpr reverse_iterator() = default;
            constexpr explicit reverse_iterator(T* ptr) : _it(ptr) {}
            constexpr explicit reverse_iterator(iterator ptr) : _it(ptr) {}
            constexpr T* get(this auto&& self) { return self.it; }

            constexpr reverse_iterator& operator++() // pre-increment
            {
                --_it;
                return *this;
            }

            constexpr reverse_iterator operator++(int) // post-increment NOLINT
            {
                auto temp = _it;
                --_it;
                return reverse_iterator{temp};
            }

            constexpr reverse_iterator& operator--() // pre-decrement
            {
                ++_it;
                return *this;
            }

            constexpr reverse_iterator operator--(int) // post-decrement NOLINT
            {
                auto temp = _it;
                ++_it;
                return reverse_iterator{temp};
            }

            constexpr T& operator*(this auto&& self) { return *self._it; }
            constexpr bool operator==(const reverse_iterator& other) const { return _it == other._it; }
            constexpr auto operator<=>(const reverse_iterator&) const = default;

        private:
            iterator _it = nullptr;
        };

        class const_reverse_iterator 
        {
        public:
            friend class const_reverse_iterator;
            
            using difference_type = std::iter_difference_t<T>;
            using value_type = T;

            constexpr const_reverse_iterator() = default;
            constexpr explicit const_reverse_iterator(T* ptr) : _it(ptr) {}
            constexpr explicit const_reverse_iterator(const_iterator ptr) : _it(ptr) {}
            constexpr const T* get(this auto&& self) { return self._location; }

            constexpr const_reverse_iterator& operator++() // pre-increment
            {
                --_it;
                return *this;
            }

            constexpr const_reverse_iterator operator++(int) // post-increment NOLINT
            {
                auto temp = _it;
                --_it;
                return const_reverse_iterator{temp};
            }

            constexpr const_reverse_iterator& operator--() // pre-decrement
            {
                ++_it;
                return *this;
            }

            constexpr const_reverse_iterator operator--(int) // post-decrement NOLINT
            {
                auto temp = _it;
                ++_it;
                return const_reverse_iterator{temp};
            }

            constexpr const T& operator*(this auto&& self) { return *self._location; }
            constexpr bool operator==(const const_reverse_iterator& other) const { return _it == other._it; }
            constexpr auto operator<=>(const const_reverse_iterator&) const = default;

        private:
            const_iterator _it = nullptr;
        };

        /**
         * @brief Construct a dyn_array with a known capacity. The objects are not initialized.
         * 
         * @param capacity The size the array is to be.
         */
        constexpr explicit dyn_array(size_t capacity)
        : _data((T*)::operator new(capacity * sizeof(T))), _capacity(capacity)
        {
        }

        /**
         * @brief Construct a dyn_array with a known capacity. The objects are initialized.
         * 
         * @param capacity The size the array is to be.
         * @param init_val The value all elements are initialized to.
         */
        constexpr dyn_array(size_t capacity, const auto& init_val)
        : _data(new T[capacity]{init_val}), _size(capacity), _capacity(capacity)
        {
        }

        constexpr dyn_array(const dyn_array& other)
        : _data((T*)::operator new(other._capacity * sizeof(T))), _size(other._size), _capacity(other._capacity)
        {
            auto it = iterator{_data};
            
            for (auto& item : other)
            {
                *it = T{item};
                ++it;
            }
        }

        constexpr dyn_array(dyn_array&& other) noexcept
        : _data(other._data), _size(other._size), _capacity(other._capacity)
        {
            other.nullify();
        }

        // Copy assignment violates the "no allocation after construction" rule.
        [[nodiscard]] constexpr dyn_array& operator=(const dyn_array&) = delete;

        [[nodiscard]] constexpr dyn_array& operator=(dyn_array&& other) noexcept
        {
            if(this == &other) { return *this; }

            delete[] _data;
            _data = other._data;
            _size = other._size;
            _capacity = other._capacity;

            other.nullify();

            return *this;
        }

        constexpr ~dyn_array()
        {
            delete[] _data;
        }

        [[nodiscard]] constexpr T& operator[](size_t index) { return _data[index]; }

        [[nodiscard]] constexpr auto& at(this auto&& self, size_t index)
        {
            if(index < self._size)
            {
                return self._data[index];
            }

            throw std::out_of_range("Index out of range.");
        }

        [[nodiscard]] constexpr T* data(this auto&& self)  { return self._data; }
        [[nodiscard]] constexpr std::span<T> to_span(this auto&& self)  { return std::span<T>{self._data, self._size}; }

        template <typename... Args>
        constexpr T& emplace_back(this auto&& self, Args&&... args)
        {
            if(self._size == self._capacity) { throw std::overflow_error("Unable to fit more items."); }

            self._data[self._size] = T{std::forward<Args>(args)...};
            ++self._size;

            return self.back();
        }

        template <typename...Args>
        constexpr T& push_back(this auto&& self, Args&&... args)
        {
            return self.emplace_back(std::forward<Args>(args)...);
        }

        constexpr T& push_back(this auto&& self, const T& item) { return self.emplace_back(item); }

        [[nodiscard]] constexpr iterator begin(this auto&& self) { return iterator{self._data}; }
        [[nodiscard]] constexpr iterator end(this auto&& self) { return iterator{self._data + self._size}; }
        [[nodiscard]] constexpr const_iterator cbegin(this auto&& self) { return const_iterator{self._data}; }
        [[nodiscard]] constexpr const_iterator cend(this auto&& self) { return const_iterator{self._data + self._size}; }

        [[nodiscard]] constexpr reverse_iterator rbegin(this auto&& self) { return reverse_iterator{self._data + (self._size - 1uz)}; }
        [[nodiscard]] constexpr reverse_iterator rend(this auto&& self) { return reverse_iterator{self._data - 1uz}; }
        [[nodiscard]] constexpr const_reverse_iterator crbegin(this auto&& self) { return const_reverse_iterator{self._data + (self._size - 1uz)}; }
        [[nodiscard]] constexpr const_reverse_iterator crend(this auto&& self) { return const_reverse_iterator{self._data - 1uz}; }

        [[nodiscard]] constexpr T& front(this auto&& self) 
        {
            if(self._size == 0uz) { throw std::out_of_range("dyn_array contains no items."); }
            return self._data[0uz]; 
        }
        [[nodiscard]] constexpr T& back(this auto&& self) 
        {
            if(self._size == 0uz) { throw std::out_of_range("dyn_array contains no items."); }
            return self._data[self._size - 1uz];
        }

        [[nodiscard]] constexpr bool empty(this auto&& self) { return self._size == 0uz; }

        [[nodiscard]] constexpr size_t size(this auto&& self) { return self._size; }
        [[nodiscard]] constexpr size_t capacity(this auto&& self) { return self._capacity; }

        constexpr void clear()
        {
            auto start = begin();
            const auto e = end();

            while(start != e)
            {
                (*start).~T();
                ++start;
            }

            _size = 0uz;
        }

        constexpr iterator erase(iterator pos)
        {
            const auto e = end();

            while(pos != e)
            {
                (*pos).~T();
                ++pos;
                --_size;
            }

            return end();
        }

        constexpr iterator erase(iterator first, iterator last)
        {
            if(first == last) { return end(); }
            
            while(first != last)
            {
                (*first).~T();
                ++first;
                --_size;
            }

            return end();
        }

    private:
        constexpr void nullify()
        {
            _data = nullptr;
            _size = 0uz;
            _capacity = 0uz;
        }

        T* _data = nullptr;
        size_t _size{}, _capacity{};
    };
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}

#endif