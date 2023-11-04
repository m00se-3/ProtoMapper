#ifndef PROTOMAPPER_CONCEPTS_HPP
#define PROTOMAPPER_CONCEPTS_HPP

#include <concepts>

namespace proto
{
	template<typename T>
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
}

#endif // !PROTOMAPPER_CONCEPTS_HPP
