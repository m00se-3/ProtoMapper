#include "Renderer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

#include <filesystem>
#include <cstdio>

Renderer::Renderer(const std::string& dir)
	: _model(glm::mat4(1.f)), _view(glm::mat4(1.f))
{
	_defaultShader.reset(new Shader());
	_defaultTexture.reset(new Texture2D());

	_defaultTexture->Create().GenerateBlank(1, 1);

	_shader = _defaultShader.get();
	_texture = _defaultTexture.get();

	/*
		Open the default shader sources and extract the text.

		TODO: Add error checking before testing.
	*/

	std::filesystem::path vs = dir + "/shaders/DefaultVS.glsl",
		fs = dir + "/shaders/DefaultFS.glsl";

	// File handles for the shader sources.

	auto* vsHandle = fopen(vs.string().c_str(), "r");
	auto* fsHandle = fopen(fs.string().c_str(), "r");

	std::string vsSrc, fsSrc;						// Strings that will hold the source text.

	vsSrc.reserve(std::filesystem::file_size(vs));
	fsSrc.reserve(std::filesystem::file_size(fs));

	/*
		Read each shader file line by line until the end.
	*/

	char buffer[50u];

	while(feof(vsHandle) == 0)
		vsSrc += fgets(buffer, 50u, vsHandle);

	while(feof(fsHandle) == 0)
		fsSrc += fgets(buffer, 50u, fsHandle);

	fclose(vsHandle);
	fclose(fsHandle);

	_defaultShader->Create(vsSrc.c_str(), fsSrc.c_str());
}

bool Renderer::Init(mode newMode)
{
	SetRenderMode(newMode);

	switch (newMode)
	{
	case mode::Two:
	{
		_projection = glm::ortho(0.f, _vWidth, _vHeight, 0.f);
		_view = glm::lookAt(glm::vec3{ _vWidth * 0.5f, _vHeight * 0.5f, 0.f }, glm::vec3{ 0.f, 0.f, -1.f }, glm::vec3{ 0.f, 1.f, 0.f });
		break;
	}
	case mode::Three:
	{
		// Make sure this is correct before testing.
		_projection = glm::perspective(glm::quarter_pi<float>(), _vWidth / _vHeight, 1.f, 10.f);
		break;
	}
	}

	_shader->Link().Cleanup();

	_shader->Bind();

	glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "model"), 1, GL_FALSE, &_model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "view"), 1, GL_FALSE, &_view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "projection"), 1, GL_FALSE, &_projection[0][0]);

	_shader->Unbind();

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
	default:
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
	default:
		break;
	}
}

Renderer::mode Renderer::GetRenderMode() const { return _currentMode; }

void Renderer::SetRenderWindow(float w, float h)
{
	_vWidth = w; _vHeight = h;
}

void Renderer::SetViewport(int x, int y, int w, int h)
{
	glViewport(x, y, w, h);
}

void Renderer::SetRenderMode(Renderer::mode m)
{
	_currentMode = m;
}

void Renderer::UseTexture(Texture2D* texture)
{
	_texture = texture;
}

void Renderer::UseShader(Shader* shader)
{
	_shader = shader;
}

void Renderer::UseBuffer(Buffer2D* buffer)
{
	_buffer = buffer;
}

void Renderer::SetUniforms(const std::function<void()>& uniforms) { _uniforms = uniforms; }

void Renderer::DrawCurrentBuffer()
{
	_buffer->Bind();
	_shader->Bind();
	_texture->Bind();
	_shader->Uniforms(_uniforms);

	glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "model"), 1, GL_FALSE, &_model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "view"), 1, GL_FALSE, &_view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(_shader->ID(), "projection"), 1, GL_FALSE, &_projection[0][0]);

	glDrawElements(GL_TRIANGLES, _buffer->GetNumberOfIndices(), GL_UNSIGNED_INT, _buffer->Indices());

	_buffer->Unbind();
	_shader->Unbind();
	_texture->Unbind();
}