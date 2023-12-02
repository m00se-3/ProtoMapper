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

#include <memory>
#include <filesystem>
#include <functional>

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_FIXED_TYPES

// We have to explicitly undefine NK_IMPLEMENTATION here to avoid linking errors.
#undef NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

#include "glad/glad.h"
#include "SimpleIni.h"

module proto.UI.Container;

import proto.Texture;
import proto.Renderer;
import proto.ResourceManager;

namespace proto
{	
	ReferenceCounter<Texture2D>* UIContainer::_texMan = nullptr;

	constexpr long long MaxVertexBuffer = 8 * 1024;

	struct NKVertex
	{
		float pos[2];
		float tex[2];
		nk_byte color[4];
	};

	static const struct nk_draw_vertex_layout_element vertex_layout[] = {
		{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct NKVertex, pos)},
		{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct NKVertex, tex)},
		{NK_VERTEX_COLOR, NK_FORMAT_B8G8R8A8, NK_OFFSETOF(struct NKVertex, color)},
		{NK_VERTEX_LAYOUT_END}
	};

	void UIContainer::SetResourceManager(ReferenceCounter<Texture2D>* ptr) { _texMan = ptr; }


	UIContainer::UIContainer()
	{
		std::string fontFile = ROOT_DIR;
		fontFile += "/assets/fonts/roboto/Roboto-Medium.ttf";
		
		int imgWidth = 0, imgHeight = 0;
		_fontTexture = _texMan->Load("Roboto-Medium", [](Texture2D& tex){ tex.Create(); });
		auto nTexture = _texMan->Get("default");
		_nullTexture.texture = nk_handle_id(static_cast<int>(nTexture.Get().ID));
		_nullTexture.uv = nk_vec2(0.0f, 0.0f);

		/*
			Initialize the UI library and the fonts.
		*/
		nk_font_atlas_init_default(&_atlas);
		nk_font_atlas_begin(&_atlas);

		_font = nk_font_atlas_add_from_file(&_atlas, fontFile.c_str(), 16.f, nullptr);

		const void* img = nk_font_atlas_bake(&_atlas, &imgWidth, &imgHeight, NK_FONT_ATLAS_RGBA32);
		_fontTexture.Get().WriteData(img, imgWidth, imgHeight);
		nk_font_atlas_end(&_atlas, nk_handle_id((int)_fontTexture.Get().ID), nullptr);

		if (!nk_init_default(&_ctx, &_font->handle)) return;
		nk_style_set_font(&_ctx, &_font->handle);

		memset(&_configurator, 0, sizeof(_configurator));
		_configurator.shape_AA = NK_ANTI_ALIASING_ON;
		_configurator.line_AA = NK_ANTI_ALIASING_ON;
		_configurator.vertex_layout = vertex_layout;
		_configurator.vertex_alignment = NK_ALIGNOF(struct NKVertex);
		_configurator.vertex_size = sizeof(struct NKVertex);
		_configurator.circle_segment_count = 20;
		_configurator.curve_segment_count = 20;
		_configurator.arc_segment_count = 20;
		_configurator.global_alpha = 1.0f;
		_configurator.null = _nullTexture;

		nk_buffer_init_default(&_cmds);

		/*
			Because I don't (yet) have the facilities to incorporate nuklear's buffer types into my own Buffer type,
			I have to set up the buffers manually.
		*/

		glGenVertexArrays(1, &_vertexArray);
		glGenBuffers(1, &_vb);
		glGenBuffers(1, &_ib);

		glBindVertexArray(_vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, _vb);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib);

		glBufferData(GL_ARRAY_BUFFER, MaxVertexBuffer, nullptr, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxVertexBuffer, nullptr, GL_DYNAMIC_DRAW);

		// Vertex positions
		glVertexAttribPointer(0u, 2, GL_FLOAT, GL_FALSE, sizeof(NKVertex), 0);
		glEnableVertexAttribArray(0u);

		// Texture coordinates
		glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(NKVertex), (const void*)(sizeof(float) * 2u));
		glEnableVertexAttribArray(1u);

		// Color values
		glVertexAttribPointer(2u, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(NKVertex), (const void*)(sizeof(float) * 4u));
		glEnableVertexAttribArray(2u);

		glBindVertexArray(0);
	}

	UIContainer::~UIContainer()
	{
		nk_font_atlas_cleanup(&_atlas);
		nk_font_atlas_clear(&_atlas);
		nk_free(&_ctx);

		_texMan->Unload("Roboto-Medium");
	}

	bool UIContainer::SetDefinitionsPath(const std::filesystem::path& filepath)
	{
		if(std::filesystem::exists(filepath))
		{
			_interfaceDir = filepath;

			return true;
		}
		
		return false;
	}

	nk_context* UIContainer::Context() { return &_ctx; }

    void UIContainer::AddFont(const std::filesystem::path& filepath)
    {
	}

	void UIContainer::Compile()
	{
		glBindVertexArray(_vertexArray);

		_vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		_indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

		nk_buffer_init_fixed(&_verts, _vertices, MaxVertexBuffer);
		nk_buffer_init_fixed(&_inds, _indices, MaxVertexBuffer);

		nk_convert(&_ctx, &_cmds, &_verts, &_inds, &_configurator);

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		glBindVertexArray(0u);

		nk_buffer_free(&_verts);
		nk_buffer_free(&_inds);
	}

	void UIContainer::Update(float wWidth, float wHeight)
	{
		if (nk_begin(&_ctx, "ProtoMapper", nk_rect(0.0f, 0.0f, wWidth * 0.5f, wHeight * 0.05f), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER))
		{
			nk_layout_row_dynamic(&_ctx, wHeight * 0.04f, 5);
			if (nk_menu_begin_label(&_ctx, "File", NK_TEXT_LEFT, nk_vec2(50.0f, wHeight * 0.1f)))
			{
				nk_layout_row_dynamic(&_ctx, wHeight * 0.04f, 1);
				nk_menu_item_label(&_ctx, "New", NK_TEXT_LEFT);

				nk_menu_end(&_ctx);
			}
		}

		nk_end(&_ctx);
	}

	void UIContainer::Draw(Renderer* ren)
	{
		/*
			Made using the examples in the nuklear repository. Made a rough API for nuklear to use with
			my Renderer class.
		*/

		const nk_draw_command* cmd = nullptr;
		const nk_draw_index* offset = nullptr;

		nk_draw_foreach(cmd, &_ctx, &_cmds)
		{
			if (!cmd->elem_count) continue;

			if(cmd->texture.id && glIsTexture((unsigned int)cmd->texture.id) == GL_TRUE)
			{			
				const auto textID = static_cast<Texture2D::IDType>(cmd->texture.id);
				ren->UseTexture(Texture2D{ textID });
			}
			else
			{
				ren->UseTexture();
			}

			ren->DrawFromExternal<unsigned int>(_vertexArray, (int)cmd->elem_count, GL_TRIANGLES, offset);
			offset += cmd->elem_count;
		}

		nk_clear(&_ctx);
		nk_buffer_clear(&_cmds);
	}
}
