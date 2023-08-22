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

#include "components/Common.hpp"

namespace proto
{
	Rect::Rect(float x, float y, float w, float h)
		: x(x), y(y), w(w), h(h)
	{
	}

	bool Rect::operator==(const Rect& other) const
	{
		return (x == other.x && y == other.y && w == other.w && h == other.h);
	}

	bool Rect::Overlaps(const Rect& other) const
	{
		return (
			x < other.x + other.w && y < other.y + other.h
			&& x + w > other.x && y + h > other.y
			);
	}

	bool Rect::Contains(const Rect& other) const
	{
		return (
			x < other.x && x + w > other.x + other.w
			&& y < other.y && y + h > other.y + other.h
			);
	}

}