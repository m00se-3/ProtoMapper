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

#ifndef PROTOMAPPER_VERTEX_HPP
#define PROTOMAPPER_VERTEX_HPP

#include "glm/glm.hpp"
#include "glad/glad.h"

#include <concepts>
#include <vector>
#include <functional>

namespace proto
{

	template<typename T> class Buffer;

	template<typename T>
	concept VertexType = requires(Buffer<T>*ptr)
	{
		{ T::Attributes(ptr) } -> std::same_as<void>;
	};

	struct Vertex2D
	{
		glm::vec2 pos;
		glm::vec2 texCoords;
		glm::vec4 color;

		static void Attributes(Buffer<Vertex2D>* buffer);
	};


	template <typename VertexType>
	class Buffer
	{
	public:
#ifdef USE_GLES
		using IndType = unsigned short;
#else
		using IndType = unsigned int;
#endif

	private:
		IndType _vID = 0u, _indID = 0u, _vao = 0u;
		bool _initialized = false;
		std::vector<VertexType> _vertices;
		std::vector<IndType> _indices;

	public:
		Buffer() = default;
		Buffer(size_t numVertices, size_t numIndices) { Generate(numVertices, numIndices); }

		void* Data() const { return (void*)_vertices.data(); }
		void* Indices() const { return (void*)_indices.data(); }

		size_t GetBufferSize() const { return _vertices.size(); }

		Buffer& Generate(size_t numVertices, size_t numIndices)
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

			VertexType::Attributes(this);

			glBindVertexArray(0);

			return *this;
		}

		Buffer& AddValues(const std::vector<VertexType>& verts, const std::vector<IndType>& inds)
		{
			for (auto& vert : verts)
			{
				_vertices.push_back(vert);
			}

			for (auto& ind : inds)
			{
				_indices.push_back(ind);
			}

			return *this;
		}

		Buffer& SetBufferSize(size_t size) { _vertices.reserve(size); return *this; }

		Buffer& SetNumberOfIndices(size_t size) { _indices.reserve(size); return *this; }

		Buffer& WriteData() {
			glBindBuffer(GL_ARRAY_BUFFER, _vID);
			glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VertexType), _vertices.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_DYNAMIC_DRAW);

			return *this;
		}

		size_t GetNumberOfIndices() const { return _indices.size(); }

		bool Bind() const
		{
			if (_initialized)
			{
				glBindVertexArray(_vao);
				return true;
			}
			return false;
		}

		void Unbind() const { glBindVertexArray(0); }
	};
}
#endif // !PROTOMAPPER_VERTEX_HPP
