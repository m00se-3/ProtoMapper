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

#include "Vertex.hpp"

void Vertex2D::Attributes(Buffer<Vertex2D>* buffer)
{
	glBufferData(GL_ARRAY_BUFFER, buffer->GetBufferSize() * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer->GetNumberOfIndices() * sizeof(Buffer<Vertex2D>::IndType), nullptr, GL_DYNAMIC_DRAW);

	buffer->Bind();

	// Vertex positions
	glVertexAttribPointer(0u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), 0);
	glEnableVertexAttribArray(0);

	// Texture coordinates
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2)));
	glEnableVertexAttribArray(1);

	// Color values
	glVertexAttribPointer(2u, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2) + sizeof(glm::vec2)));
	glEnableVertexAttribArray(2);
}

void NKVertex::Attributes(Buffer<NKVertex>* buffer)
{
	glBufferData(GL_ARRAY_BUFFER, buffer->GetBufferSize() * sizeof(NKVertex), nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffer->GetNumberOfIndices() * sizeof(Buffer<NKVertex>::IndType), nullptr, GL_DYNAMIC_DRAW);

	buffer->Bind();

	// Vertex positions
	glVertexAttribPointer(0u, 2, GL_FLOAT, GL_FALSE, sizeof(NKVertex), 0);
	glEnableVertexAttribArray(0);

	// Texture coordinates
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(NKVertex), (const void*)(sizeof(float) * 2));
	glEnableVertexAttribArray(1);

	// Color values
	glVertexAttribPointer(2u, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(NKVertex), (const void*)(sizeof(float) * 4));
	glEnableVertexAttribArray(2);
}