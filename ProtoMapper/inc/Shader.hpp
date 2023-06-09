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

#ifndef PROTOMAPPER_SHADER_HPP
#define PROTOMAPPER_SHADER_HPP

#include "glad/glad.h"

#include <functional>

class ResourceManager;

struct Shader
{
	using IDType = unsigned int;
	
	IDType ID = 0u;

	Shader() = default;
	Shader(const Shader&);
	explicit Shader(IDType id);
	~Shader();

	bool operator==(const Shader& rhs);
	bool operator==(const Shader& rhs) const;
	Shader& operator=(const Shader& rhs);

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
	void Bind();
	void Bind() const;
	void Unbind();
	void Unbind() const;
	void Destroy();

	// Pass in a lambda that sets up the uniforms for the shader.
	void Uniforms(const std::function<void()>& func);


	static void SetResourceManager(ResourceManager* ptr);

private:
	static ResourceManager* _manager;

};

#endif // !PROTOMAPPER_SHADER_HPP
