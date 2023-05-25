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