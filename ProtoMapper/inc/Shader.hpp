#ifndef PROTOMAPPER_SHADER_HPP
#define PROTOMAPPER_SHADER_HPP

#include "glad/glad.h"

#include <functional>

struct Shader
{
	unsigned int ID = 0u;

	Shader() = default;
	Shader(const Shader&) = default;
	~Shader();

	std::pair<unsigned int, unsigned int> CreateBasic(const char* srcVert, const char* srcFrag);
	unsigned int Attach(const char* src, unsigned int type);
	void Link();
	void Link(const std::pair<unsigned int, unsigned int>& shaders);
	void Bind() const;
	void Unbind() const;
	void Destroy();

	// Pass in a lambda that sets up the uniforms for the shader.
	Shader& Uniforms(const std::function<void()>& func);
};

#endif // !PROTOMAPPER_SHADER_HPP
