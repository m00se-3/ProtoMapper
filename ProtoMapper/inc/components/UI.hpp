#ifndef PROTOMAPPER_COMPONENTS_UI_HPP
#define PROTOMAPPER_COMPONENTS_UI_HPP

#include "Common.hpp"
#include "entt/entt.hpp"

#include <vector>

// Forward declarations.
struct nk_context;

/*
	A std::vector of entities that represent the various elements contained within a UI container.
*/
struct UIStructure
{
	std::vector<entt::entity> entities;

	UIStructure() = default;
	UIStructure(const UIStructure&) = delete;
	UIStructure(UIStructure&& other);
};

/*
	A rectangle representing the area occupied by the UI element.
*/
struct UIBoundingBox
{
	struct Rectangle area;

	UIBoundingBox() = default;
	UIBoundingBox(const UIBoundingBox&) = default;

};


#endif // !PROTOMAPPER_COMPONENTS_UI_HPP
