#include "Components.hpp"

UIElement::UIElement(const std::function<FunctionType>& fun)
	: func(fun)
{
}

UIElement::UIElement(const UIElement& other)
	: func(other.func)
{
}
