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
module;

#include <cstdint>
#include <utility>
#include <functional>
#include <print>

#include "glad/glad.h"

module proto.Shader;

namespace proto
{	
	std::pair<Shader::IDType, Shader::IDType> Shader::CreateBasic(const char* srcVert, const char* srcFrag)
	{
		ID = glCreateProgram();

		unsigned int vs = Attach(srcVert, GL_VERTEX_SHADER);
		unsigned int fs = Attach(srcFrag, GL_FRAGMENT_SHADER);

		glDeleteShader(vs);
		glDeleteShader(fs);


		return std::make_pair(vs, fs);
	}

	unsigned int Shader::Attach(const char* src, Shader::IDType type)
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

			std::println("Vertex shader failed to compile: {}\n", buffer);

			return 0u;
		}

#endif // _DEBUG_

		glAttachShader(ID, shader);

		return shader;
	}

	Shader::IDType Shader::GetID() const { return ID; }

	bool Shader::Valid() const { return ID != 0; }

	void Shader::Reset() { ID = 0u; }

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

			std::println("Could not link shader program: {}\n", buffer);
		}
#endif
	}

	void Shader::Link(const std::pair<Shader::IDType, Shader::IDType>& shaders)
	{
		auto [vs, fs] = shaders;

		Link();

		glDetachShader(ID, vs);
		glDetachShader(ID, fs);
	}

	void Shader::Bind() { glUseProgram(ID); }

	void Shader::Bind() const { glUseProgram(ID); }

	void Shader::Unbind() { glUseProgram(0); }

	void Shader::Unbind() const { glUseProgram(0); }

	void Shader::Destroy() { glDeleteProgram(ID); }


	void Shader::Uniforms(const std::function<void()>& func)
	{
		func();
	}

	Shader::Shader(const Shader& other)
		:ID(other.ID)
	{
	}

	Shader::Shader(IDType id)
		:ID(id)
	{
	}

	Shader::~Shader() {  }

	bool Shader::operator==(const Shader& rhs) const
	{
		return ID == rhs.ID;
	}

	Shader& Shader::operator=(const Shader& rhs)
	{
		ID = rhs.ID;

		return *this;
	}
}
