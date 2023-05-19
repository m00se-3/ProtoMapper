#include "Vertex.hpp"


#include "glad/glad.h"

Buffer2D::Buffer2D(size_t numVertices, size_t numIndices) { Generate(numVertices, numIndices); }

void* Buffer2D::Data() const { return (void*)_vertices.data(); }

void* Buffer2D::Indices() const { return (void*)_indices.data(); }

Buffer2D& Buffer2D::AddValues(const std::vector<Vertex2D>& vertices, const std::vector<IndType>& indices)
{
	for (auto& vert : vertices)
	{
		_vertices.push_back(vert);
	}

	for (auto& ind : indices)
	{
		_indices.push_back(ind);
	}

	return *this;
}

Buffer2D& Buffer2D::Generate(size_t numVertices, size_t numIndices)
{
	if (!_initialized)
	{
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vID);
		glGenBuffers(1, &_indID);
		_initialized = true;
	}

	_vertices.reserve(numVertices);
	_indices.reserve(numIndices);

	glBindBuffer(GL_ARRAY_BUFFER, _vID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indID);

	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), nullptr, GL_DYNAMIC_DRAW);

	glBindVertexArray(_vao);

	// Vertex positions
	glVertexAttribPointer(0u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), 0);
	glEnableVertexAttribArray(0);

	// Texture coordinates
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2)));
	glEnableVertexAttribArray(1);

	// Color values
	glVertexAttribPointer(2u, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2) +sizeof(glm::vec2)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	return *this;
}

Buffer2D& Buffer2D::WriteData()
{	
	glBindBuffer(GL_ARRAY_BUFFER, _vID);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex2D), _vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_DYNAMIC_DRAW);

	return *this;
}

Buffer2D& Buffer2D::SetBufferSize(size_t size) { _vertices.reserve(size); return *this; }

Buffer2D& Buffer2D::SetNumberOfIndices(size_t size) { _indices.reserve(size); return *this; }

size_t Buffer2D::GetNumberOfIndices() const { return _indices.size(); }

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