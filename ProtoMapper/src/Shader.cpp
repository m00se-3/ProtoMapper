#include "Shader.hpp"

Shader& Shader::Create(const char* srcVert, const char* srcFrag)
{
	auto vs = glCreateShader(GL_VERTEX_SHADER);
	auto fs = glCreateShader(GL_FRAGMENT_SHADER);
	_ID = glCreateProgram();

	glShaderSource(GL_VERTEX_SHADER, 1, &srcVert, nullptr);
	glShaderSource(GL_FRAGMENT_SHADER, 1, &srcFrag, nullptr);

	glCompileShader(vs);
	glCompileShader(fs);

	glAttachShader(_ID, vs);
	glAttachShader(_ID, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return *this;
}

void Shader::Bind() const { glUseProgram(_ID); }

void Shader::Destroy() { glDeleteProgram(_ID); }

Shader& Shader::Uniforms(const std::function<void()>& func)
{
	func();
	return *this;
}