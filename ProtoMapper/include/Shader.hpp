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
#ifndef PROTO_SHADER_HPP
#define PROTO_SHADER_HPP

#include <cstdint>
#include <utility>
#include <functional>

#include <glad/glad.h>

namespace proto
{
    struct Shader
	{
		using IDType = uint32_t;

		IDType ID = 0u;

		Shader() = default;
		explicit Shader(IDType id);

		bool operator==(const Shader& rhs) const = default;

		[[nodiscard]] IDType GetID() const;
		[[nodiscard]] bool Valid() const;

		/*
			Create a simple shader program from the traditional vertex and fragment shader combo.

			Returns the ids to the shader objects, if any are 0 something went wrong.
		*/
		std::pair<IDType, IDType> CreateBasic(const char* srcVert, const char* srcFrag);

		// Resets the current reference object to 0.
		void Reset();

		/*
			Attach an individual shader object and return the id.
		*/
		unsigned int Attach(const char* src, IDType type) const;

		/*
			Final linking stage of creating an OpenGL shader.
		*/
		void Link() const;

		/*
			Same as Link(), but deletes and detaches the vertex and fragment shader pair after finished.
		*/
		void Link(const std::pair<IDType, IDType>& shaders) const;
		void Bind(this auto&& self) { glUseProgram(self.ID); }
		static void Unbind();
		void Destroy() const;

		// Pass in a lambda that sets up the uniforms for the shader.
		static void Uniforms(const std::function<void()>& func);

	};

} // namespace proto

#endif
