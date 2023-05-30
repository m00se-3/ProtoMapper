#ifndef PROTOMAPPER_COMPONENTS_HPP
#define PROTOMAPPER_COMPONENTS_HPP

#include <functional>

struct nk_context;	// Forward declaration.

/*
	Defines the area occupied by the SceneNode. Any position values should be relative to this.
*/
struct Rectangle
{
	float x = 0.f, y = 0.f, w = 0.f, h = 0.f;

	Rectangle() = default;
	Rectangle(float x, float y, float w, float h);

	bool operator==(const Rectangle& other) const;

	bool Overlaps(const Rectangle& other) const;
	bool Contains(const Rectangle& other) const;
};


struct UIElement
{
	using FunctionType = void(nk_context*, const struct Rectangle&);
	
	std::function<FunctionType> func = [](nk_context*, const struct Rectangle&) {};

	UIElement() = default;
	UIElement(const std::function<FunctionType>& fun);
	UIElement(const UIElement& other);
};

#endif // !PROTOMAPPER_COMPONENTS_HPP
