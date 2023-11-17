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
module;

#include <optional>
#include <string>
#include <filesystem>
#include <array>
#include <functional>

export module proto.Renderer;

import "glad/glad.h";
import "glm/glm.hpp";
import "glm/gtc/matrix_transform.hpp";

import proto.Vertex;
import proto.Texture;
import proto.Shader;
import proto.ResourceManager;

namespace proto
{

	export class Renderer
	{
	public:
		enum class mode
		{
			Two,
			Three
		};

		Renderer(const std::string& dir);
		~Renderer();

		mode GetRenderMode() const;

		bool Init(mode newMode);
		void Begin();
		void End();

		void SetRenderSize(int w, int h);
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
	};
	
	Renderer::Renderer(const std::string& dir)
		: _model(glm::mat4(1.f)), _view(glm::mat4(1.f)), _projection(glm::mat4(1.f)), _currentTexture(std::nullopt), _currentShader(std::nullopt)
	{

		_defaultTexture.Create().GenerateBlank(1, 1);

		/*
			Open the default shader sources and extract the text.
		*/

		std::filesystem::path vs = dir + "/assets/shaders/DefaultVS.glsl",
			fs = dir + "/assets/shaders/DefaultFS.glsl";

		// File handles for the shader sources.


		std::FILE* vsHandle = std::fopen(vs.string().c_str(), "r");
		std::FILE* fsHandle = std::fopen(fs.string().c_str(), "r");

		if (vsHandle && fsHandle)
		{

			std::string vsSrc, fsSrc;						// Strings that will hold the source text.

			vsSrc.reserve(std::filesystem::file_size(vs));
			fsSrc.reserve(std::filesystem::file_size(fs));

			/*
				Read each shader file line by line until the end.
			*/

			std::array<char, 50u> buffer = { 0 };

			while (std::feof(vsHandle) == 0)
				vsSrc += std::fgets(buffer.data(), 50u, vsHandle);

			std::memset(buffer.data(), 0, 50u);	// Reset the buffer memory to ensure no garbage data is present.

			while (std::feof(fsHandle) == 0)
				fsSrc += std::fgets(buffer.data(), 50u, fsHandle);

			std::fclose(vsHandle);
			std::fclose(fsHandle);

			auto objs = _defaultShader.CreateBasic(vsSrc.c_str(), fsSrc.c_str());
			_defaultShader.Link(objs);
		}
	}

	bool Renderer::Init(mode newMode)
	{
		SetRenderMode(newMode);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		return true;
	}

	int Renderer::GetRenderWidth() const { return _vWidth; }

	int Renderer::GetRenderHeight() const { return _vHeight; }

	int Renderer::GetRenderX() const { return _vX; }

	int Renderer::GetRenderY() const { return _vY; }

	void Renderer::Begin()
	{
		switch (_currentMode)
		{
		case Renderer::mode::Two:
		{
			glDisable(GL_DEPTH_TEST);
			break;
		}
		case Renderer::mode::Three:
			break;
		}
	}

	void Renderer::End()
	{
		switch (_currentMode)
		{
		case Renderer::mode::Two:
		{
			//glEnable(GL_DEPTH_TEST);
			break;
		}
		case Renderer::mode::Three:
			break;
		}
	}

	Renderer::mode Renderer::GetRenderMode() const { return _currentMode; }

	void Renderer::SetRenderSize(int w, int h)
	{
		_vWidth = w; _vHeight = h;
	}

	void Renderer::SetViewport(int x, int y, int w, int h)
	{
		_vX = x; _vY = y;

		/*
			We want to set the viewport using the top-left as the point of origin, but openGL uses the bottom-left.

			We quickly transpose the 'y' value to match up with what the user wants.
		*/
		glViewport(_vX, (_vHeight - (h + _vY)), w, h);
	}

	void Renderer::SetRenderMode(Renderer::mode m)
	{
		_currentMode = m;

		switch (m)
		{
		case mode::Two:
		{
			_projection = glm::ortho(0.f, (float)_vWidth, (float)_vHeight, 0.f);
			_view = glm::lookAt(glm::vec3{ 0.0f, 0.0f, 0.f }, glm::vec3{ 0.f, 0.f, -1.f }, glm::vec3{ 0.f, 1.f, 0.f });
			_model = glm::mat4(1.0f);
			break;
		}
		case mode::Three:
		{
			// Make sure this is correct before testing.
			_projection = glm::perspective(glm::quarter_pi<float>(), (float)_vWidth / (float)_vHeight, 1.f, 10.f);
			break;
		}
		}
	}

	void Renderer::UseTexture(std::optional<Texture2D> texture)
	{
		if (texture)
		{
			_currentTexture = texture;
		}
		else
		{
			_currentTexture = std::nullopt;
		}
	}

	void Renderer::UseShader(std::optional<Shader> shader)
	{
		if (shader)
		{
			_currentShader = shader;
		}
		else
		{
			_currentShader = std::nullopt;
		}
	}

	void Renderer::SetUniforms(const std::function<void()>& uniforms) { _uniforms = uniforms; }

	Renderer::~Renderer()
	{
		if (_currentTexture) _currentTexture->Destroy();
		if (_currentShader) _currentShader->Destroy();

		_defaultTexture.Destroy();
		_defaultShader.Destroy();
	}
}
