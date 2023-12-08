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
#ifndef PROTO_RENDERER_HPP
#define PROTO_RENDERER_HPP

#include <optional>
#include <string>
#include <filesystem>
#include <array>
#include <functional>
#include <memory>
#include <cstdio>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Vertex.hpp"
#include "Texture.hpp"
#include "Shader.hpp"
#include "ResourceManager.hpp"

namespace proto
{

	class Renderer
	{
	public:
		enum class mode
		{
			Two,
			Three
		};

		Renderer(const std::string& dir);
		~Renderer();

		static void SetResourceManager(ResourceManager* ptr);

		mode GetRenderMode() const;

		bool Init(mode newMode);
		void Begin();
		void End();

		void SetRenderSize(int w, int h);
		void RefreshProjection();
		int GetRenderWidth() const;
		int GetRenderHeight() const;
		int GetRenderX() const;
		int GetRenderY() const;

		void SetUniforms(const std::function<void()>& uniforms);
		void SetViewport(int x, int y, int w, int h);
		void SetRenderMode(mode m);
		void UseTexture(std::optional<Texture2D> texture = std::nullopt);
		void UseShader(std::optional<Shader> shader = std::nullopt);

		template<typename VertexType>
		const unsigned int* DrawBuffer(Buffer<VertexType>& buffer, std::optional<unsigned int> indexCount = std::nullopt, const unsigned int* offset = nullptr, unsigned int drawMode = GL_TRIANGLES)
		{
			unsigned int GLIndexType = 0u;

			const auto elements = indexCount.value_or(buffer.GetNumberOfIndices());

			if constexpr (std::is_same_v<typename Buffer<VertexType>::IndType, uint32_t>)
			{
				GLIndexType = GL_UNSIGNED_INT;
			}
			else
			{
				GLIndexType = GL_UNSIGNED_SHORT;
			}

			auto& shader = (_currentShader) ? *_currentShader : _defaultShader;
			auto& texture = (_currentTexture) ? *_currentTexture : _defaultTexture;

			buffer.Bind();
			texture.Bind();
			shader.Bind();
			shader.Uniforms(_uniforms);

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &_model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &_view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &_projection[0][0]);
			glUniform1i(glGetUniformLocation(shader.ID, "textureData"), 0);

			glDrawElements(drawMode, elements, GLIndexType, offset);

			texture.Unbind();
			shader.Unbind();
			buffer.Unbind();

			return offset + elements;
		}

		template<typename IndexType, typename OffsetType>
		const unsigned int* DrawFromExternal(unsigned int vertexArray, int numInds, unsigned int drawMode = GL_TRIANGLES, const OffsetType* offset = nullptr)
		{
			unsigned int GLIndexType = 0u;

			if constexpr (std::is_same_v<IndexType, uint32_t>)
			{
				GLIndexType = GL_UNSIGNED_INT;
			}
			else
			{
				GLIndexType = GL_UNSIGNED_SHORT;
			}

			auto& shader = (_currentShader) ? *_currentShader : _defaultShader;
			auto& texture = (_currentTexture) ? *_currentTexture : _defaultTexture;

			glBindVertexArray(vertexArray);
			texture.Bind();
			shader.Bind();
			shader.Uniforms(_uniforms);

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &_model[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &_view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, &_projection[0][0]);
			glUniform1i(glGetUniformLocation(shader.ID, "textureData"), 0);

			glDrawElements(drawMode, numInds, GLIndexType, offset);

			glBindVertexArray(0u);

			texture.Unbind();
			shader.Unbind();

			return offset + numInds;
		}

	private:
		glm::mat4 _model, _view, _projection;
		mode _currentMode = mode::Two;
		int _vX = 0, _vY = 0, _vWidth = 0, _vHeight = 0;

		std::optional<Texture2D> _currentTexture;
		std::optional<Shader> _currentShader;
		Shader _defaultShader;
		Texture2D _defaultTexture;

		std::function<void()> _uniforms = []() {};

		static ResourceManager* _resources;
	};

}

#endif