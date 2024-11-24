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
#include <filesystem>
#include <functional>
#include <queue>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <Texture.hpp>
#include <Shader.hpp>

namespace proto
{

	// Represents a single draw call.
	struct DrawCall
	{
		uint32_t buffer = 0u, drawMode = GL_TRIANGLES;
		int32_t elemCount = 0;
		uint32_t offset = 0u;
		std::optional<Texture2D> texture = std::nullopt;
	};
	
	class Renderer
	{
	public:
		enum class mode : uint8_t
		{
			Two,
			Three
		};

		Renderer(const std::filesystem::path& dir);

		[[nodiscard]] constexpr mode GetRenderMode(this auto&& self) { return self._currentMode; }

		bool Init(mode newMode);
		void Begin();
		void End(std::span<DrawCall> uiDrawCalls);

		static void SetBackgroundColor(const glm::vec4& col);
		void SetRenderSize(int w, int h);
		void RefreshProjection();
		[[nodiscard]] constexpr auto GetRenderWidth(this auto&& self) { return self._vWidth; }
		[[nodiscard]] constexpr auto GetRenderHeight(this auto&& self) { return self._vHeight; }
		[[nodiscard]] constexpr auto GetRenderX(this auto&& self) { return self._vX; }
		[[nodiscard]] constexpr auto GetRenderY(this auto&& self) { return self._vY; }
		[[nodiscard]] constexpr auto GetDefaultTexture(this auto&& self) { return self._defaultTexture; }

		void SetUniforms(const std::function<void()>& uniforms);
		void SetViewport(int x, int y, int w, int h);
		void SetRenderMode(mode m);
		void UseTexture(std::optional<Texture2D> texture = std::nullopt);
		void UseShader(std::optional<Shader> shader = std::nullopt);
		void PushDrawCall(const DrawCall& call);
		void PushDrawCallRange(const std::span<DrawCall>& range);

	private:

		template<typename IndexType, typename OffsetType>
		void Draw(unsigned int vertexArray, int numInds, OffsetType offset = 0u, unsigned int drawMode = GL_TRIANGLES)
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
			Shader::Uniforms(_uniforms);

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(_model));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, glm::value_ptr(_view));
			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
			glUniform1i(glGetUniformLocation(shader.ID, "textureData"), 0);

			// NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast, performance-no-int-to-ptr) - no other choice.
			glDrawElements(drawMode, numInds, GLIndexType, (const void*)offset);

			glBindVertexArray(0u);

			Texture2D::Unbind();
			Shader::Unbind();
		}

		glm::mat4 _model, _view, _projection;
		mode _currentMode = mode::Two;
		int _vX = 0, _vY = 0, _vWidth = 0, _vHeight = 0;

		std::optional<Texture2D> _currentTexture;
		std::optional<Shader> _currentShader;
		Shader _defaultShader;
		Texture2D _defaultTexture;

		std::function<void()> _uniforms = []() {};
		std::queue<DrawCall> _drawQueue;
	};

}

#endif
