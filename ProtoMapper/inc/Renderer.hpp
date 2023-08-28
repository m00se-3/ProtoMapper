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

#ifndef PROTOMAPPER_RENDERER_HPP
#define PROTOMAPPER_RENDERER_HPP

#include "Vertex.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <string>
#include <functional>
#include <type_traits>
#include <optional>

namespace proto
{

	class ResourceManager;
	class Texture2DManager;
	class ShaderManager;

	class Renderer
	{
	public:
		enum class mode
		{
			Two,
			Three
		};

	private:
		glm::mat4 _model, _view, _projection;
		mode _currentMode = mode::Two;
		float _vWidth = 0.f, _vHeight = 0.f;

		std::optional<Texture2D> _currentTexture;
		std::optional<Shader> _currentShader;
		Shader _defaultShader;
		Texture2D _defaultTexture;

		std::function<void()> _uniforms = []() {};

		/*
			Thus far, Renderer only needs these on creation, when it needs to construct
			it's default shader and texture.

			This could be a canidate for further optimization.
		*/

		static Texture2DManager* _texMan;
		static ShaderManager* _shadMan;

	public:

		Renderer(const std::string& dir);
		~Renderer();

		static void SetResourceManager(ResourceManager* res);

		mode GetRenderMode() const;

		bool Init(mode newMode);
		void Begin();
		void End();
		void SetRenderWindow(float w, float h);
		float GetRenderWidth() const;
		float GetRenderHeight() const;
		void SetUniforms(const std::function<void()>& uniforms);
		void SetViewport(int x, int y, int w, int h);
		void SetRenderMode(mode m);
		void UseTexture(std::optional<Texture2D> texture = std::nullopt);
		void UseShader(std::optional<Shader> shader = std::nullopt);

		template<typename VertexType>
		void DrawBuffer(const Buffer<VertexType>& buffer, unsigned int drawMode = GL_TRIANGLES)
		{
			unsigned int GLIndexType = 0u;

			if constexpr (std::is_same_v<typename Buffer<VertexType>::IndType, unsigned int>)
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

			glDrawElements(drawMode, buffer.GetNumberOfIndices(), GLIndexType, nullptr);

			texture.Unbind();
			shader.Unbind();
			buffer.Unbind();
		}

		template<typename IndexType, typename OffsetType>
		void DrawFromExternal(unsigned int vertexArray, int numInds, unsigned int drawMode = GL_TRIANGLES, const OffsetType* offset = nullptr)
		{
			unsigned int GLIndexType = 0u;

			if constexpr (std::is_same_v<IndexType, unsigned int>)
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
		}
	};
}
#endif // !PROTOMAPPER_RENDERER_HPP
