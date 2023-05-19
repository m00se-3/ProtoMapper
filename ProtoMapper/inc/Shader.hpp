#ifndef PROTOMAPPER_SHADER_HPP
#define PROTOMAPPER_SHADER_HPP

#include "glad/glad.h"

#include <functional>

class Shader
{
	unsigned int _ID = 0u, _vs = 0u, _fs = 0u;

public:
	Shader() = default;
	~Shader();

	Shader& Create(const char* srcVert, const char* srcFrag);
	Shader& Link();
	Shader& Cleanup();
	void Bind() const;
	void Unbind() const;
	unsigned int ID() const;
	void Destroy();

	// Pass in a lambda that sets up the uniforms for the shader.
	Shader& Uniforms(const std::function<void()>& func);
};

#endif // !PROTOMAPPER_SHADER_HPP
