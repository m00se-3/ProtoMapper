#ifndef PROTOMAPPER_VERTEX_HPP
#define PROTOMAPPER_VERTEX_HPP

#include "glm/glm.hpp"

#include <vector>

struct Vertex2D
{
	glm::vec2 pos;
	glm::vec2 texCoords;
	glm::vec4 color;
};

class Buffer2D
{
	unsigned int _vID = 0u, _indID = 0u, _vao = 0u;
	bool _initialized = false;
	std::vector<Vertex2D> _vertices;
	std::vector<unsigned int> _indices;

public:
	Buffer2D() = default;
	Buffer2D(size_t numVertices, size_t numIndices);

	void* Data() const;
	void* Indices() const;

	void Generate(size_t numVertices, size_t numIndices);
	void SetBufferSize(size_t size);
	void SetNumberOfIndices(size_t size);
	void WriteData();
	bool Bind();
	void Unbind();
};

#endif // !PROTOMAPPER_VERTEX_HPP
