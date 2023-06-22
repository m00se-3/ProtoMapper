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

#ifndef PROTOMAPPER_COMPONENTS_UI_HPP
#define PROTOMAPPER_COMPONENTS_UI_HPP

#include "Common.hpp"

#include <string_view>

// Forward declarations.
struct nk_context;

/*
	Represents a window and it's identifier.
*/
struct UIWindow
{
    std::string_view id, title;

	UIWindow() = default;
	UIWindow(const UIWindow&) = delete;
	UIWindow(UIWindow&& other);
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
