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
#include "Renderer.hpp"

#include <filesystem>

#include <fstream>
#include <gsl/gsl-lite.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace proto
{
	
	Renderer::Renderer(const std::filesystem::path& dir)
		: _model(glm::mat4(1.f)), _view(glm::mat4(1.f)), _projection(glm::mat4(1.f)),
		_currentTexture(std::nullopt), _currentShader(std::nullopt)
	{
		_defaultTexture.Create().GenerateBlank(1, 1);

		const std::filesystem::path vs = dir / "assets/shaders/DefaultVS.glsl",
			fs = dir / "assets/shaders/DefaultFS.glsl";

		auto streamVS = std::fstream{ vs.string().c_str(), std::ios_base::in };
		auto streamFS = std::fstream{ fs.string().c_str(), std::ios_base::in };

		if (streamVS.is_open() && streamFS.is_open())
		{
			std::string vsSrc, fsSrc;

			vsSrc.reserve(std::filesystem::file_size(vs));
			fsSrc.reserve(std::filesystem::file_size(fs));

			for(std::string line; std::getline(streamVS, line, '\t');) 
			{
				vsSrc += line;
			}

			for(std::string line; std::getline(streamFS, line, '\t');) 
			{
				fsSrc += line;
			}

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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::End(std::span<DrawCall> uiDrawCalls)
	{
		PushDrawCallRange(uiDrawCalls);
		
		while (!_drawQueue.empty())
		{
			auto& call = _drawQueue.front();

			UseTexture(call.texture);

			Draw<uint32_t>(call.buffer, call.elemCount, call.offset, call.drawMode);

			_drawQueue.pop();
		}
	}

	void Renderer::SetBackgroundColor(const glm::vec4& col)
	{
		glClearColor(col.r, col.g, col.b, col.a); // NOLINT: union access is unavoidable
	}

	void Renderer::SetRenderSize(int w, int h)
	{
		_vWidth = w; _vHeight = h;
	}

	void Renderer::RefreshProjection()
	{
		switch (_currentMode)
		{
		case mode::Two:
		{
			_projection = glm::ortho(0.f, (float)_vWidth, (float)_vHeight, 0.f);
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

	void Renderer::PushDrawCall(const DrawCall& call)
	{
		_drawQueue.push(call);
	}

	void Renderer::PushDrawCallRange(const std::span<DrawCall>& range)
	{
		for (auto& call : range)
		{
			_drawQueue.push(call);
		}
	}
}
