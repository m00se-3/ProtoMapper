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
#include <Shader.hpp>

#include <array>

namespace proto
{	
	static constexpr size_t readBufferLength = 100z;

	std::pair<Shader::IDType, Shader::IDType> Shader::CreateBasic(const char* srcVert, const char* srcFrag)
	{
		ID = glCreateProgram();

		unsigned int vs = Attach(srcVert, GL_VERTEX_SHADER);
		unsigned int fs = Attach(srcFrag, GL_FRAGMENT_SHADER);

		glDeleteShader(vs);
		glDeleteShader(fs);


		return std::make_pair(vs, fs);
	}

	unsigned int Shader::Attach(const char* src, Shader::IDType type) const
	{
		unsigned int shader = glCreateShader(type);

		glShaderSource(shader, 1, &src, nullptr);
		glCompileShader(shader);

#ifdef _DEBUG_

		int test{}, length{};

		glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		if (test == 0)
		{
			std::array<char, readBufferLength> buffer{};

			glGetShaderInfoLog(shader, readBufferLength, &length, buffer.data());

			std::puts("Vertex shader failed to compile.\n");

			return 0u;
		}

#endif // _DEBUG_

		glAttachShader(ID, shader);

		return shader;
	}

	Shader::IDType Shader::GetID() const { return ID; }

	bool Shader::Valid() const { return ID != 0; }

	void Shader::Reset() { ID = 0u; }

	void Shader::Link() const
	{
		glLinkProgram(ID);

#ifdef _DEBUG_
		glValidateProgram(ID);

		int result{};
		glGetProgramiv(ID, GL_VALIDATE_STATUS, &result);

		if (result == 0)
		{
			int length{};
			glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &length);

			std::array<char, readBufferLength> buffer{};

			glGetProgramInfoLog(ID, readBufferLength, &length, buffer.data());

			std::puts("Could not link shader program.\n");
		}
#endif
	}

	void Shader::Link(const std::pair<Shader::IDType, Shader::IDType>& shaders) const
	{
		auto [vs, fs] = shaders;

		Link();

		glDetachShader(ID, vs);
		glDetachShader(ID, fs);
	}
	void Shader::Unbind() { glUseProgram(0); }

	void Shader::Destroy() const { glDeleteProgram(ID); }


	void Shader::Uniforms(const std::function<void()>& func)
	{
		func();
	}

	Shader::Shader(IDType id)
		:ID(id)
	{
	}
}
