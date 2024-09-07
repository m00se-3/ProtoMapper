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
#ifndef PROTO_VERTEX_HPP
#define PROTO_VERTEX_HPP

#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace proto
{
    template<typename T>
	concept VertexType = requires()
	{
		{ T::Attributes() } -> std::same_as<void>;
	};

	struct Vertex2D
	{
		glm::vec2 pos;
		glm::vec2 texCoords;
		glm::vec4 color;

		static void Attributes();
	};


	template <VertexType VType>
	class Buffer
	{
	public:
		using IndType = uint32_t;

		Buffer() = default;
		Buffer(size_t numVertices, size_t numIndices) { Generate(numVertices, numIndices); }

		[[nodiscard]] void* Data() const { return (void*)_vertices.data(); }
		[[nodiscard]] void* Indices() const { return (void*)_indices.data(); }

		[[nodiscard]] IndType VAO() const { return _vao; }

		[[nodiscard]] size_t GetBufferSize() const { return _vertices.size(); }

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

			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, _vID);
			glBufferData(GL_ARRAY_BUFFER, _vertices.capacity() * sizeof(VType), nullptr, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.capacity() * sizeof(IndType), nullptr, GL_DYNAMIC_DRAW);

			VType::Attributes();

			glBindVertexArray(0);

			return *this;
		}

		/*
			Don't forget to write the data to the GPU with a call to WriteData().
		*/
		Buffer& AddValues(const std::vector<VType>& verts, const std::vector<IndType>& inds)
		{
			for (auto& vert : verts)
			{
				_vertices.push_back(vert);
			}

			for (const auto& ind : inds)
			{
				_indices.push_back(ind);
			}

			return *this;
		}

		Buffer& SetBufferSize(size_t size) { _vertices.reserve(size); return *this; }

		Buffer& SetNumberOfIndices(size_t size) { _indices.reserve(size); return *this; }

		Buffer& WriteData() {
			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, _vID);
			glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(VType), _vertices.data(), GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(IndType), _indices.data(), GL_DYNAMIC_DRAW);
			glBindVertexArray(0);

			return *this;
		}

		[[nodiscard]] size_t GetNumberOfIndices() const { return _indices.size(); }

		void Bind() const
		{
			if (_initialized)
			{
				glBindVertexArray(_vao);
			}
		}

		void Unbind() const { glBindVertexArray(0); }

		void Clear() { _vertices.clear(); _indices.clear(); }

	private:
		IndType _vID = 0u, _indID = 0u, _vao = 0u;
		bool _initialized = false;
		std::vector<VType> _vertices;
		std::vector<IndType> _indices;

	
	};
}

#endif
