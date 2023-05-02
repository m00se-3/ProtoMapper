#ifndef PROTOMAPPER_SHADER_HPP
#define PROTOMAPPER_SHADER_HPP

#include "glad/glad.h"

#include <functional>

class Shader
{
	unsigned int _ID = 0u;

public:
	Shader() = default;

	Shader& Create(const char* srcVert, const char* srcFrag);
	void Bind() const;
	void Destroy();

	// Pass in a lambda that sets up the uniforms for the shader.
	Shader& Uniforms(const std::function<void()>& func);
};

#endif // !PROTOMAPPER_SHADER_HPP
