#include "Renderer.hpp"

#include <filesystem>
#include <cstdio>

Renderer::Renderer(const std::string& dir)
	: _model(glm::mat4(1.f)), _view(glm::mat4(1.f)), _currentTexture(std::nullopt), _currentShader(std::nullopt)
{

	_defaultTexture.Create().GenerateBlank(1, 1);

	/*
		Open the default shader sources and extract the text.
	*/

	std::filesystem::path vs = dir + "/shaders/DefaultVS.glsl",
		fs = dir + "/shaders/DefaultFS.glsl";

	// File handles for the shader sources.

	FILE* vsHandle;
	FILE* fsHandle;

	fopen_s(&vsHandle, vs.string().c_str(), "r");
	fopen_s(&fsHandle, fs.string().c_str(), "r");

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

	auto objs = _defaultShader.CreateBasic(vsSrc.c_str(), fsSrc.c_str());
	_defaultShader.Link(objs);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	/*
		We want to set the viewport using the top-left as the point of origin, but openGL uses the bottom-left.

		We quickly transpose the 'y' value to match up with what the user wants.
	*/
	glViewport(x, (int)_vHeight - (h + y), w, h);
}

void Renderer::SetRenderMode(Renderer::mode m)
{
	_currentMode = m;
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
	_defaultTexture.Destroy();
	_defaultShader.Destroy();
}