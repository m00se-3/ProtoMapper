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

#include "glad/glad.h"

#include <functional>

export module Shader;

import Concepts;
export namespace proto
{
	export struct Shader
	{
		using IDType = uint32_t;

		IDType ID = 0u;

		Shader() = default;

		Shader(const Shader& other)
			:ID(other.ID)
		{
		}

		explicit Shader(IDType id)
			:ID(id)
		{
		}

		~Shader() = default;

		bool operator==(const Shader& rhs) const
		{
			return ID == rhs.ID;
		}

		Shader& operator=(const Shader& rhs)
		{
			ID = rhs.ID;

			return *this;
		}

		IDType GetID() const { return ID; }

		bool Valid() const { return ID != 0; }

		/*
			Create a simple shader program from the traditional vertex and fragment shader combo.

			Returns the ids to the shader objects, if any are 0 something went wrong.
		*/
		std::pair<IDType, IDType> CreateBasic(const char* srcVert, const char* srcFrag)
		{
			ID = glCreateProgram();

			unsigned int vs = Attach(srcVert, GL_VERTEX_SHADER);
			unsigned int fs = Attach(srcFrag, GL_FRAGMENT_SHADER);

			glDeleteShader(vs);
			glDeleteShader(fs);


			return std::make_pair(vs, fs);
		}

		// Resets the current reference object to 0.
		void Reset() { ID = 0u; }

		/*
			Attach an individual shader object and return the id.
		*/
		unsigned int Attach(const char* src, IDType type)
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

		/*
			Final linking stage of creating an OpenGL shader.
		*/
		void Link()
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

		/*
			Same as Link(), but deletes and detaches the vertex and fragment shader pair after finished.
		*/
		void Link(const std::pair<IDType, IDType>& shaders)
		{
			auto [vs, fs] = shaders;

			Link();

			glDetachShader(ID, vs);
			glDetachShader(ID, fs);
		}

		void Bind() { glUseProgram(ID); }

		void Bind() const { glUseProgram(ID); }

		void Unbind() { glUseProgram(0); }
		
		void Unbind() const { glUseProgram(0); }

		void Destroy() { glDeleteProgram(ID); }

		// Pass in a lambda that sets up the uniforms for the shader.
		void Uniforms(const std::function<void()>& func)
		{
			func();
		}

	};	
}
