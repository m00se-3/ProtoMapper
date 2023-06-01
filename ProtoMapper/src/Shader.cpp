#include "Shader.hpp"

std::pair<unsigned int, unsigned int> Shader::CreateBasic(const char* srcVert, const char* srcFrag)
{
	ID = glCreateProgram();

	unsigned int vs = Attach(srcVert, GL_VERTEX_SHADER);
	unsigned int fs = Attach(srcFrag, GL_FRAGMENT_SHADER);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return std::make_pair(vs, fs);
}

unsigned int Shader::Attach(const char* src, unsigned int type)
{
	unsigned int shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

#ifdef _DEBUG_

		int test, length;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		
		if (!test)
		{
			char buffer[100u];

			glGetShaderInfoLog(shader, 100u, &length, buffer);
			
			printf_s("Vertex shader failed to compile: %s\n", buffer);

			return 0u;
		}

#endif // _DEBUG_

		glAttachShader(ID, shader);

		return shader;
}

void Shader::Link() 
{
	glLinkProgram(ID);

#ifdef _DEBUG_
	glValidateProgram(ID);

	int result;
	glGetProgramiv(ID, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		int length;
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length);

		char buffer[100u];

		glGetProgramInfoLog(ID, 100u, &length, buffer);

		printf_s("Could not link shader program: %s\n", buffer);
	}
#endif
}

void Shader::Link(const std::pair<unsigned int, unsigned int>& shaders)
{
	auto [vs, fs] = shaders;

	Link();

	glDetachShader(ID, vs);
	glDetachShader(ID, fs);
}

void Shader::Bind() const { glUseProgram(ID); }

void Shader::Unbind() const { glUseProgram(0); }

void Shader::Destroy() { glDeleteProgram(ID); }


Shader& Shader::Uniforms(const std::function<void()>& func)
{
	func();
	return *this;
}

Shader::~Shader() { Destroy(); }