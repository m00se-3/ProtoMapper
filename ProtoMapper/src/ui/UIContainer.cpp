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
#include "UIContainer.hpp"

#include "ProtoMapper.hpp"

namespace proto
{	
	ReferenceCounter<Texture2D>* UIContainer::_texMan = nullptr;

	constexpr long long MaxVertexBuffer = 8 * 1024;

	/*
		Special Vertex struct that has a nuklear friendly layout and alignment.
	*/
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


	UIContainer::UIContainer(const std::string& assetsDir)
	{
		std::string fontDir =  assetsDir + "/fonts/roboto/";
		std::filesystem::path imgDir = assetsDir + "/icons/";

		if(std::filesystem::exists(imgDir))
		{
			for(auto& icon : std::filesystem::directory_iterator(imgDir))
			{
				auto& file = icon.path();

				Image img{file};
				auto iconImg = _texMan->Load(file.stem().string(), [](Texture2D& tex){ tex.Create(); });
				iconImg.Get().WriteImage(img);
			}
		}
		

		auto nTexture = _texMan->Get("default");
		_nullTexture.texture = nk_handle_id(static_cast<int>(nTexture.Get().ID));
		_nullTexture.uv = nk_vec2(0.0f, 0.0f);

		// Add Fonts to the FontGroup.

		_fonts.Create();

		_fonts.AddFont(FontStyle::Normal, 16.0f, fontDir + "Roboto-Medium.ttf");
		_fonts.AddFont(FontStyle::Bold, 16.0f, fontDir + "Roboto-Bold.ttf");
		_fonts.AddFont(FontStyle::BoldItalic, 16.0f, fontDir + "Roboto-BoldItalic.ttf");
		_fonts.AddFont(FontStyle::Italic, 16.0f, fontDir + "Roboto-Italic.ttf");

		/*
            Because nk_font_atlas_end() frees the img pointer for us, we can't include a Bake function
			in the FontGroup class that returns an Image.

			As a result, it's just easier to handle baking this way.
        */

		int imgWidth = 0, imgHeight = 0;
        const void* img = nk_font_atlas_bake(_fonts.GetAtlas(), &imgWidth, &imgHeight, NK_FONT_ATLAS_RGBA32);

		// Create Texture object.

		_fontTexture = _texMan->Load("Roboto-Medium", [](Texture2D& tex){ tex.Create(); });
		_fontTexture.Get().WriteData(img, imgWidth, imgHeight);

		_fonts.Finalize(_fontTexture.Get().GetID());


		if (!nk_init_default(&_ctx, &_fonts.GetFont(FontStyle::Normal)->handle)) return;

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
		DrawCustomTitleBar(wWidth);
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
				ren->UseTexture(Texture2D{ static_cast<Texture2D::IDType>(cmd->texture.id) });
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

	void UIContainer::DrawCustomTitleBar(float width)
	{
		//auto& closeImg = _texMan->Get("iconClose").Get();
		
		if(nk_begin(&_ctx, "ProtoMapper", nk_rect(0.0f, 0.0f, width, 50.0f), NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BORDER))
		{
			nk_layout_space_begin(&_ctx, NK_STATIC, 20.0f, 2);

			nk_layout_space_push(&_ctx, nk_rect(0.0f, 0.0f, width - 100.0f, 25.0f));
			nk_label(&_ctx, "ProtoMapper", NK_TEXT_LEFT);
			
			nk_layout_space_push(&_ctx, nk_rect(width - 35.f, 0.0f, 25.f, 20.0f));
			//if(nk_button_image(&_ctx, nk_subimage_id(closeImg.GetID(), 24, 24, nk_rect(0, 0, 24, 24 ))))
			if(nk_button_label(&_ctx, "X"))
			{
				glfwSetWindowShouldClose(Mapper::GetInstance()->GetWin().GetPtr(), 1);
			}

			nk_layout_space_end(&_ctx);			
		
			nk_layout_space_begin(&_ctx, NK_STATIC, 20.0f, 1);

			nk_layout_space_push(&_ctx, nk_rect(0.0f, 0.0f, 50.0f, 20.0f));
			if (nk_menu_begin_label(&_ctx, "File", NK_TEXT_LEFT, nk_vec2(100.0f, 60.0f)))
			{
				nk_layout_row_static(&_ctx, 20.0f, 90, 1);
				nk_menu_item_label(&_ctx, "New", NK_TEXT_LEFT);

				nk_menu_end(&_ctx);
			}

			nk_layout_space_end(&_ctx);

		}

		nk_end(&_ctx);
	}

	void UIContainer::InitLua()
	{
		// Useful types
		{

			auto vec2 = _lua.new_usertype<struct nk_vec2>("vec2", sol::no_constructor);
			vec2["x"] = &nk_vec2::x;
			vec2["y"] = &nk_vec2::y;
			_lua["new_vec2"] = nk_vec2;

			auto vec2i = _lua.new_usertype<struct nk_vec2i>("vec2i", sol::no_constructor);
			vec2i["x"] = &nk_vec2i::x;
			vec2i["y"] = &nk_vec2i::y;
			_lua["new_vec2i"] = nk_vec2i;

			auto rect = _lua.new_usertype<struct nk_rect>("rect", sol::no_constructor);
			rect["x"] = &nk_rect::x;
			rect["y"] = &nk_rect::y;
			_lua["new_rect"] = nk_rect;

			auto recti = _lua.new_usertype<struct nk_recti>("recti", sol::no_constructor);
			recti["x"] = &nk_recti::x;
			recti["y"] = &nk_recti::y;
			_lua["new_recti"] = nk_recti;

			auto color = _lua.new_usertype<struct nk_color>("color", sol::no_constructor);
			color["r"] = &nk_color::r;
			color["g"] = &nk_color::g;
			color["b"] = &nk_color::b;
			color["a"] = &nk_color::a;

			auto colorf = _lua.new_usertype<struct nk_colorf>("colorf", sol::no_constructor);
			colorf["r"] = &nk_colorf::r;
			colorf["g"] = &nk_colorf::g;
			colorf["b"] = &nk_colorf::b;
			colorf["a"] = &nk_colorf::a;


			_lua.new_enum<FontStyle>( "FontStyle",
				{
					std::make_pair("Normal", FontStyle::Normal),
					std::make_pair("Bold", FontStyle::Bold),
					std::make_pair("Italic", FontStyle::Italic),
					std::make_pair("Underlined", FontStyle::Underlined),
					std::make_pair("BoldItalic", FontStyle::BoldItalic),
					std::make_pair("BoldUnderlinded", FontStyle::BoldUnderlinded),
					std::make_pair("BolItalicUnderlined", FontStyle::BolItalicUnderlined),
					std::make_pair("ItalicUnderlined", FontStyle::ItalicUnderlined)
				}
			);


			_lua.new_enum<nk_flags>( "TextAlign",
				{
					std::make_pair("Left", NK_TEXT_LEFT),
					std::make_pair("Center", NK_TEXT_CENTERED),
					std::make_pair("Right", NK_TEXT_RIGHT)
				}
				
			);


			_lua.new_enum<nk_panel_flags>( "PanelFlag",
				{
					std::make_pair("Border", NK_WINDOW_BORDER),
					std::make_pair("Movable", NK_WINDOW_MOVABLE),
					std::make_pair("Scalable", NK_WINDOW_SCALABLE),
					std::make_pair("Closable", NK_WINDOW_CLOSABLE),
					std::make_pair("Minimizable", NK_WINDOW_MINIMIZABLE),
					std::make_pair("NoScrollbar", NK_WINDOW_NO_SCROLLBAR),
					std::make_pair("Title", NK_WINDOW_TITLE),
					std::make_pair("ScrollAutoHide", NK_WINDOW_SCROLL_AUTO_HIDE),
					std::make_pair("Background", NK_WINDOW_BACKGROUND),
					std::make_pair("ScaleLeft", NK_WINDOW_SCALE_LEFT),
					std::make_pair("NoInput", NK_WINDOW_NO_INPUT)
				}
			);


			_lua.new_enum<nk_layout_format>("Layout",
				{
					std::make_pair("Dynamic", NK_DYNAMIC),
					std::make_pair("Static", NK_STATIC)
				}
			);

		}


		// Access to the nuklear context.

		_lua["Ctx"] = &_ctx;
		_lua["Ctx"]["style"] = &_ctx.style;

		/*
			Define nuklear functions.
		*/

		// Windows

		_lua["Begin"] = nk_begin;
		_lua["End"] = nk_end;

		// Layouts

		_lua["SpaceRowBegin"] = nk_layout_space_begin;
		_lua["SpaceRowEnd"] = nk_layout_space_end;
		_lua["SpaceRowPush"] = nk_layout_space_push;
		_lua["StaticRow"] = nk_layout_row_static;
		_lua["DynamicRow"] = nk_layout_row_dynamic;

		//Widgets

		_lua["BeginMenuLbl"] = nk_menu_begin_label;
		_lua["BeginMenuTxt"] = nk_menu_begin_text;
		_lua["BeginMenuImg"] = nk_menu_begin_image;
		_lua["BeginMenuImgTxt"] = nk_menu_begin_image_text;
		_lua["BeginMenuImgLbl"] = nk_menu_begin_image_label;
		_lua["BeginMenuSym"] = nk_menu_begin_symbol;
		_lua["BeginMenuSymTxt"] = nk_menu_begin_symbol_text;
		_lua["BeginMenuSymLbl"] = nk_menu_begin_symbol_label;
		_lua["MenuItemTxt"] = nk_menu_item_text;
		_lua["MenuItemLbl"] = nk_menu_item_label;
		_lua["MenuItemImgLbl"] = nk_menu_item_image_label;
		_lua["MenuItemImgTxt"] = nk_menu_item_image_text;
		_lua["MenuItemSymTxt"] = nk_menu_item_symbol_text;
		_lua["MenuItemSymLbl"] = nk_menu_item_symbol_label;
		_lua["CloeMenu"] = nk_menu_close;
		_lua["EndMenu"] = nk_menu_end;

		_lua["ButtonTxt"] = nk_button_text;
		_lua["ButtonLbl"] = nk_button_label;
		_lua["ButtonC"] = nk_button_color;
		_lua["ButtonSym"] = nk_button_symbol;
		_lua["ButtonImg"] = nk_button_image;
		_lua["ButtonSymLbl"] = nk_button_symbol_label;
		_lua["ButtonSymTxt"] = nk_button_symbol_text;
		_lua["ButtonImgLbl"] = nk_button_image_label;
		_lua["ButtonImgTxt"] = nk_button_image_text;
		_lua["ButtonTxtSty"] = nk_button_text_styled;
		_lua["ButtonLblSty"] = nk_button_label_styled;
		_lua["ButtonSymSty"] = nk_button_symbol_styled;
		_lua["ButtonImgSty"] = nk_button_image_styled;
		_lua["ButtonSymTxtSty"] = nk_button_symbol_text_styled;
		_lua["ButtonSymLblSty"] = nk_button_symbol_label_styled;
		_lua["ButtonImgLblSty"] = nk_button_image_label_styled;
		_lua["ButtonImgTxtSty"] = nk_button_image_text_styled;

		_lua["CheckLbl"] = nk_check_label;
		_lua["CheckTxt"] = nk_check_text;
		_lua["CheckFlagLbl"] = nk_check_flags_label;
		_lua["CheckFlagTxt"] = nk_check_flags_text;
		_lua["CheckboxLbl"] = nk_checkbox_label;
		_lua["CheckboxTxt"] = nk_checkbox_text;
		_lua["CheckboxFlagLbl"] = nk_checkbox_flags_label;
		_lua["CheckboxFlagTxt"] = nk_checkbox_flags_text;

		_lua["RadioLbl"] = nk_radio_label;
		_lua["RadioTxt"] = nk_radio_text;
		_lua["RadioOptLbl"] = nk_option_label;
		_lua["RadioOptTxt"] = nk_option_text;

		_lua["SelectableLbl"] = nk_selectable_label;
		_lua["SelectableTxt"] = nk_selectable_text;
		_lua["SelectableImgLbl"] = nk_selectable_image_label;
		_lua["SelectableImgTxt"] = nk_selectable_image_text;
		_lua["SelectableSymLbl"] = nk_selectable_symbol_label;
		_lua["SelectableSymTxt"] = nk_selectable_symbol_text;

		_lua["SelectLbl"] = nk_select_label;
		_lua["SelectTxt"] = nk_select_text;
		_lua["SelectImgLbl"] = nk_select_image_label;
		_lua["SelectImgTxt"] = nk_select_image_text;
		_lua["SelectSymLbl"] = nk_select_symbol_label;
		_lua["SelectSymTxt"] = nk_select_symbol_text;

		_lua["SlideF"] = nk_slide_float;
		_lua["SlideI"] = nk_slide_int;
		_lua["SliderF"] = nk_slider_float;
		_lua["SliderI"] = nk_slider_int;

		_lua["Progress"] = nk_progress;
		_lua["Prog"] = nk_prog;

		_lua["ColorPicker"] = nk_color_picker;
		_lua["PickColor"] = nk_color_pick;

		_lua["PopupBegin"] = nk_popup_begin;
		_lua["PopupClose"] = nk_popup_close;
		_lua["PopupEnd"] = nk_popup_end;
		_lua["PopupGetScr"] = nk_popup_get_scroll;
		_lua["PopupSetScr"] = nk_popup_set_scroll;

		_lua["Combo"] = nk_combo;
		_lua["ComboSep"] = nk_combo_separator;
		_lua["ComboStr"] = nk_combo_string;
		_lua["ComboCallb"] = nk_combo_callback;
		_lua["Combobox"] = nk_combobox;
		_lua["ComboboxStr"] = nk_combobox_string;
		_lua["ComboboxSep"] = nk_combobox_separator;
		_lua["ComboboxCallb"] = nk_combobox_callback;

		_lua["ContextBegin"] = nk_contextual_begin;
		_lua["ContextItemTxt"] = nk_contextual_item_text;
		_lua["ContextItemLbl"] = nk_contextual_item_label;
		_lua["ContextItemImgLbl"] = nk_contextual_item_image_label;
		_lua["ContextItemImgTxt"] = nk_contextual_item_image_text;
		_lua["ContextItemSymLbl"] = nk_contextual_item_symbol_label;
		_lua["ContextItemSymTxt"] = nk_contextual_item_symbol_text;
		_lua["ContextClose"] = nk_contextual_close;
		_lua["ContextEnd"] = nk_contextual_end;

		_lua["TooltipTxt"] = nk_tooltip;
		_lua["TooltipBegin"] = nk_tooltip_begin;
		_lua["TooltipEnd"] = nk_tooltip_end;

		// Styles

		_lua["StylePushFont"] = [this](sol::optional<FontStyle> style) {
				if (style)
				{
					nk_style_push_font(&_ctx, &_fonts.GetFont(*style)->handle);
				}
		};

		_lua["StylePopFont"] = nk_style_pop_font;

	}	
}
