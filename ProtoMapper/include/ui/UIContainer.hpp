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
#ifndef PROTO_UI_CONTAINER
#define PROTO_UI_CONTAINER

#include <filesystem>

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_FIXED_TYPES

// We have to explicitly undefine NK_IMPLEMENTATION here to avoid linking errors.
#undef NK_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "sol/sol.hpp"

#include "Texture.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Font.hpp"

namespace proto
{
	class UIContainer
	{
	public:
		UIContainer(const std::string& assetsDir);
		~UIContainer();

		[[nodiscard]]bool SetDefinitions(const std::filesystem::path& filepath);
		[[nodiscard]]nk_context* Context();

		void Update(float wWidth, float wHeight); // This is the big function.
		void Compile();
		void Draw(Renderer* ren);
		void ShowAboutWindow(nk_context* ctx);

		static void SetResourceManager(ReferenceCounter<Texture2D>* ptr);

	private:

		void InitLua();

		std::filesystem::path _interfaceDir;
		sol::state _lua;
		
		/*
			All things below are necessary for nuklear to work and are, mostly, taken from the documentation.
		*/

		struct nk_context _ctx;
		struct nk_convert_config _configurator;
		struct nk_buffer _cmds, _verts, _inds;
		struct nk_draw_null_texture _nullTexture;
		GPUResource<Texture2D> _fontTexture;
		FontGroup _fonts;
		

		unsigned int _vertexArray = 0u, _vb = 0u, _ib = 0u;
		void* _vertices = nullptr, * _indices = nullptr;

		/*
			End nuklear buffer variables.
		*/

		static ReferenceCounter<Texture2D>* _texMan;

	};

}

#endif