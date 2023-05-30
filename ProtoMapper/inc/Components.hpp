#ifndef PROTOMAPPER_COMPONENTS_HPP
#define PROTOMAPPER_COMPONENTS_HPP

#include <functional>

struct nk_context;	// Forward declaration.

struct UIElement
{
	using FunctionType = void(nk_context*, float, float, float, float);
	
	std::function<FunctionType> func = [](nk_context*, float, float, float, float) {};

	UIElement() = default;
	UIElement(const std::function<FunctionType>& fun);
	UIElement(const UIElement& other);
};

#endif // !PROTOMAPPER_COMPONENTS_HPP
