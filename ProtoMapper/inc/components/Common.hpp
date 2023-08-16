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
