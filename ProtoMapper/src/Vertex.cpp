#include "Vertex.hpp"

#include "glad/glad.h"

Buffer2D::Buffer2D(size_t numVertices, size_t numIndices) { Generate(numVertices, numIndices); }

void* Buffer2D::Data() const { return (void*)_vertices.data(); }

void* Buffer2D::Indices() const { return (void*)_indices.data(); }

Buffer2D& Buffer2D::Generate(size_t numVertices, size_t numIndices)
{
	if (!_initialized)
	{
		glGenBuffers(1, &_vID);
		glGenBuffers(1, &_indID);
		glGenVertexArrays(1, &_vao);
		_initialized = true;
	}

	_vertices.reserve(numVertices);
	_indices.reserve(numIndices);

	glBindVertexArray(_vao);

	WriteData();

	// Vertex positions
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), 0);

	// Texture coordinates
	glVertexAttribPointer(2u, 2, GL_UNSIGNED_INT, GL_TRUE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2)));

	// Color values
	glVertexAttribPointer(3, 4, GL_UNSIGNED_INT, GL_TRUE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2) +sizeof(glm::vec2)));

	return *this;
}

Buffer2D& Buffer2D::WriteData()
{
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex2D), _vertices.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(Vertex2D), _indices.data(), GL_DYNAMIC_DRAW);

	return *this;
}

Buffer2D& Buffer2D::SetBufferSize(size_t size) { _vertices.reserve(size); return *this; }

Buffer2D& Buffer2D::SetNumberOfIndices(size_t size) { _indices.reserve(size); return *this; }

bool Buffer2D::Bind()
{
	if (_initialized)
	{
		glBindVertexArray(_vao);
		return true;
	}
	return false;
}

void Buffer2D::Unbind() { glBindVertexArray(0); }