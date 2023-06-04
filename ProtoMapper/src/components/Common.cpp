#include "components/Common.hpp"

Rectangle::Rectangle(float x, float y, float w, float h)
	: x(x), y(y), w(w), h(h)
{
}

bool Rectangle::operator==(const Rectangle& other) const
{
	return (x == other.x && y == other.y && w == other.w && h == other.h);
}

bool Rectangle::Overlaps(const Rectangle& other) const
{
	return (
		x < other.x + other.w && y < other.y + other.h
		&& x + w > other.x && y + h > other.y
		);
}

bool Rectangle::Contains(const Rectangle& other) const
{
	return (
		x < other.x && x + w > other.x + other.w
		&& y < other.y && y + h > other.y + other.h
		);
}