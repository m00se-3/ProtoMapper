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

	/*
		Create a simple shader program from the traditional vertex and fragment shader combo.

		Returns the ids to the shader objects, if any are 0 something went wrong.
	*/
	std::pair<unsigned int, unsigned int> CreateBasic(const char* srcVert, const char* srcFrag);

	/*
		Attach an individual shader object and return the id.
	*/
	unsigned int Attach(const char* src, unsigned int type);

	/*
		Final linking stage of creating an OpenGL shader.
	*/
	void Link();

	/*
		Same as Link(), but deletes and detaches the vertex and fragment shader pair after finished.
	*/
	void Link(const std::pair<unsigned int, unsigned int>& shaders);
	void Bind() const;
	void Unbind() const;
	void Destroy();

	// Pass in a lambda that sets up the uniforms for the shader.
	Shader& Uniforms(const std::function<void()>& func);
};

#endif // !PROTOMAPPER_SHADER_HPP
