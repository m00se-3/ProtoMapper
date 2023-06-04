#ifndef PROTOMAPPER_COMMON_HPP
#define PROTOMAPPER_COMMON_HPP

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



#endif // !PROTOMAPPER_COMMON_HPP
