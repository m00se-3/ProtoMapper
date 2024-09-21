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
#include <map>

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
#include "resource.hpp"
#include "Font.hpp"
#include "Vertex.hpp"
#include "Window.hpp"
#include "System.hpp"

namespace proto
{
	class UIContainer : public System
	{
	public:
		UIContainer(FontGroup& fonts, Window *win, Renderer *ren);

		// Defines each UI function for the application to use.
		[[nodiscard]] bool SetDefinitions(const std::filesystem::path &filepath);
		void InitLua(gsl::not_null<sol::state*> ptr);
		[[nodiscard]] constexpr bool IsLua(this auto&& self) { return (self._lua != nullptr); }

		[[nodiscard]] nk_context *Context() { return _ctx.get(); }
		[[nodiscard]] bool IsActive() const override { return true; }

		// Calls each UI Lua function and reports any errors.
		void Update([[maybe_unused]] float dt) override;

		[[nodiscard]] std::span<DrawCall> Compile();

	private:
		struct CtxDeleter
		{
		    void operator()(struct nk_context* ctx) { nk_free(ctx); }
		};

		std::filesystem::path _interfaceDir;
		sol::state* _lua = nullptr;
		sol::table _dimensions;

		std::map<std::string, std::string> _luaFunctions;
		std::map<std::string, shared_res<Texture2D>> _icons;
		std::vector<DrawCall> _drawCalls;
		
		/*
			All things below are necessary for nuklear to work and are, mostly, taken from the documentation.
		*/

		std::unique_ptr<struct nk_context, CtxDeleter> _ctx;
		struct nk_convert_config _configurator;
		struct nk_buffer _cmds, _verts, _inds;
		struct nk_draw_null_texture _nullTexture;
		Texture2D _fontTexture;
		
		Buffer<Vertex2D> _nkBuffer;

		/*unsigned int _vertexArray = 0u, _vb = 0u, _ib = 0u;
		void* _vertices = nullptr, * _indices = nullptr;*/

		/*
			End nuklear buffer variables.
		*/

		Window* _window = nullptr;
	};

}

#endif
