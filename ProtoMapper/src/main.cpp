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
#include <ProtoMapper.hpp>

int main()
{
	proto::Mapper mapper;

	if (!mapper.Configure())
	{
		std::puts("Failed to load application configurations. Please reinstall the program.");
		return EXIT_FAILURE;
	}

	auto result = mapper.Run(); // TODO: Expand this return type to print out error messages.

	return result;
}
