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
#ifdef USE_GLES
	using IndType = unsigned short;
#else
	using IndType = unsigned int;
#endif

	IndType _vID = 0u, _indID = 0u, _vao = 0u;
	bool _initialized = false;
	std::vector<Vertex2D> _vertices;
	std::vector<IndType> _indices;

public:
	Buffer2D() = default;
	Buffer2D(size_t numVertices, size_t numIndices);

	void* Data() const;
	void* Indices() const;

	Buffer2D& Generate(size_t numVertices, size_t numIndices);
	Buffer2D& AddValues(const std::vector<Vertex2D>& verts, const std::vector<IndType>& inds);
	Buffer2D& SetBufferSize(size_t size);
	Buffer2D& SetNumberOfIndices(size_t size);
	Buffer2D& WriteData();
	bool Bind();
	void Unbind();
};

#endif // !PROTOMAPPER_VERTEX_HPP
