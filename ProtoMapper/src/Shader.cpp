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

#include "Shader.hpp"
#include "ResourceManager.hpp"

namespace proto
{

	ShaderManager* Shader::_manager = nullptr;

	void Shader::SetResourceManager(ShaderManager* ptr)
	{
		_manager = ptr;
	}

	std::pair<unsigned int, unsigned int> Shader::CreateBasic(const char* srcVert, const char* srcFrag)
	{
		ID = glCreateProgram();

		unsigned int vs = Attach(srcVert, GL_VERTEX_SHADER);
		unsigned int fs = Attach(srcFrag, GL_FRAGMENT_SHADER);

		glDeleteShader(vs);
		glDeleteShader(fs);

		_manager->AddReference(ID);

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
		_manager->AddReference(ID);
	}

	Shader::Shader(IDType id)
		:ID(id)
	{
		_manager->AddReference(ID);
	}

	Shader::~Shader() { if (_manager->SubReference(ID) == 0u) Destroy(); }

	bool Shader::operator==(const Shader& rhs)
	{
		return ID == rhs.ID;
	}

	bool Shader::operator==(const Shader& rhs) const
	{
		return ID == rhs.ID;
	}

	Shader& Shader::operator=(const Shader& rhs)
	{
		if (ID != 0u) _manager->SubReference(ID);

		ID = rhs.ID;
		_manager->AddReference(ID);

		return *this;
	}
}
