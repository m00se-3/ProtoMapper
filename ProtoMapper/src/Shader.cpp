#include "Shader.hpp"



Shader& Shader::Create(const char* srcVert, const char* srcFrag)
{
	_vs = glCreateShader(GL_VERTEX_SHADER);
	_fs = glCreateShader(GL_FRAGMENT_SHADER);
	_ID = glCreateProgram();

	glShaderSource(_vs, 1, &srcVert, nullptr);
	glShaderSource(_fs, 1, &srcFrag, nullptr);

	glCompileShader(_vs);
	glCompileShader(_fs);

	glAttachShader(_ID, _vs);
	glAttachShader(_ID, _fs);

#ifdef _DEBUG_

	int testVert, testFrag, vLength, fLength;

	glGetShaderiv(_vs, GL_COMPILE_STATUS, &testVert);
	glGetShaderiv(_fs, GL_COMPILE_STATUS, &testFrag);
	glGetShaderiv(_vs, GL_INFO_LOG_LENGTH, &vLength);
	glGetShaderiv(_fs, GL_INFO_LOG_LENGTH, &fLength);

	char vBuffer[100u], fBuffer[100u];

	glGetShaderInfoLog(_vs, 100u, &vLength, vBuffer);
	glGetShaderInfoLog(_fs, 100u, &fLength, fBuffer);

	if (!testVert) printf_s("Vertex shader failed to compile: %s\n", vBuffer);
	if (!testFrag) printf_s("Fragment shader failed to compile: %s\n", fBuffer);

#endif // _DEBUG_


	return *this;
}

Shader& Shader::Link() 
{
	glLinkProgram(_ID);

#ifdef _DEBUG_
	glValidateProgram(_ID);

	int result;
	glGetProgramiv(_ID, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		int length;
		glGetProgramiv(_ID, GL_INFO_LOG_LENGTH, &length);

		char buffer[100u];

		glGetProgramInfoLog(_ID, 100u, &length, buffer);

		printf_s("Could not link shader program: %s\n", buffer);
	}
#endif

	return *this;
}

Shader& Shader::Cleanup()
{
	glDeleteShader(_vs);
	glDeleteShader(_fs);

	return *this;
}

void Shader::Bind() const { glUseProgram(_ID); }

void Shader::Unbind() const { glUseProgram(0); }

void Shader::Destroy() { glDeleteProgram(_ID); }

unsigned int Shader::ID() const { return _ID; }

Shader& Shader::Uniforms(const std::function<void()>& func)
{
	func();
	return *this;
}

Shader::~Shader() { Destroy(); }