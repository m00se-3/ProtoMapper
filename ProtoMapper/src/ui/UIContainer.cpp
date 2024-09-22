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

#include <cstdio>
#include <array>

#include "ProtoMapper.hpp"
#include "Config.hpp"

namespace proto
{	
	/*
		NOTE: If you experience rendering issues while building the ui, consider increasing this buffer size.
	*/

	static constexpr uint64_t MaxVertexBuffer = 32ull * 1024ull;

	static constexpr std::array<struct nk_draw_vertex_layout_element, 4z> vertex_layout = {
		nk_draw_vertex_layout_element{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex2D, pos)},
		nk_draw_vertex_layout_element{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(Vertex2D, texCoords)},
		nk_draw_vertex_layout_element{NK_VERTEX_COLOR, NK_FORMAT_R32G32B32A32_FLOAT, NK_OFFSETOF(Vertex2D, color)},
		nk_draw_vertex_layout_element{NK_VERTEX_LAYOUT_END}
	};

	UIContainer::UIContainer(FontGroup& fonts, Window* win, Renderer* ren)
		: _ctx(new nk_context, CtxDeleter{}), _configurator(), _cmds(), _verts(), _inds(), _nullTexture(), _window(win)
	{
		const std::string fontDir = GetAssetDir() + "/fonts/roboto/";
		const std::filesystem::path imgDir = GetAssetDir() + "/icons/";

		if(std::filesystem::exists(imgDir))
		{
			for(const auto& icon : std::filesystem::directory_iterator(imgDir))
			{
				const auto& file = icon.path();

				Image img{file};
				auto iconImg = _icons.insert_or_assign(file.stem().string(), make_res<Texture2D>([](Texture2D& tex){ tex.Destroy(); }));
				iconImg.first->second.get().Create().WriteImage(img);
			}
		}
		

		auto nTexture = ren->GetDefaultTexture();
		_nullTexture.texture = nk_handle_id(static_cast<int>(nTexture.ID));
		_nullTexture.uv = nk_vec2(0.0f, 0.0f);

		// Add Fonts to the FontGroup.

		fonts.Create();

		fonts.AddFont(FontStyle::Normal, 16.0f, fontDir + "Roboto-Medium.ttf");
		fonts.AddFont(FontStyle::Bold, 16.0f, fontDir + "Roboto-Bold.ttf");
		fonts.AddFont(FontStyle::BoldItalic, 16.0f, fontDir + "Roboto-BoldItalic.ttf");
		fonts.AddFont(FontStyle::Italic, 16.0f, fontDir + "Roboto-Italic.ttf");

		/*
            Because nk_font_atlas_end() frees the img pointer for us, we can't include a Bake function
			in the FontGroup class that returns an Image.

			As a result, it's just easier to handle baking this way.
        */

		int imgWidth{}, imgHeight{};
	    const void* img = nk_font_atlas_bake(fonts.GetAtlas(), &imgWidth, &imgHeight, NK_FONT_ATLAS_RGBA32);

		// Create Texture object.
		_fontTexture.Create().WriteData(img, imgWidth, imgHeight);

		fonts.Finalize(_fontTexture.GetID());

		if (nk_init_default(_ctx.get(), &fonts.GetFont(FontStyle::Normal)->handle) == 0)
		{
		    return;
		}

		_configurator.shape_AA = NK_ANTI_ALIASING_ON;
		_configurator.line_AA = NK_ANTI_ALIASING_ON;
		_configurator.vertex_layout = vertex_layout.data();
		_configurator.vertex_alignment = NK_ALIGNOF(Vertex2D);
		_configurator.vertex_size = sizeof(Vertex2D);
		_configurator.circle_segment_count = 20;
		_configurator.curve_segment_count = 20;
		_configurator.arc_segment_count = 20;
		_configurator.global_alpha = 1.0f;
		_configurator.null = _nullTexture;

		nk_buffer_init_default(&_cmds);

		_nkBuffer.Generate(MaxVertexBuffer, MaxVertexBuffer);
		
	}

	bool UIContainer::SetDefinitions(const std::filesystem::path& filepath)
	{
		if(IsLua() && std::filesystem::exists(filepath))
		{
			_interfaceDir = filepath;

			for (const auto& iterator : std::filesystem::directory_iterator(_interfaceDir))
			{
				const auto& file = iterator.path();

				if (file.extension() == ".lua")
				{
					const sol::protected_function_result result = _lua->script_file(file.string());

					if (result.valid())
					{
						const std::pair<std::string, std::string> stuff = result;
						_luaFunctions.insert_or_assign(stuff.first, stuff.second);
					}
				}
			}

			return true;
		}
		
		return false;
	}

	std::span<DrawCall> UIContainer::Compile()
	{
		_drawCalls.clear();
		_nkBuffer.Bind();

		void* verts = _nkBuffer.Data(), * inds = _nkBuffer.Indices();

		verts = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		inds = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

		nk_buffer_init_fixed(&_verts, verts, MaxVertexBuffer);
		nk_buffer_init_fixed(&_inds, inds, MaxVertexBuffer);

		nk_convert(_ctx.get(), &_cmds, &_verts, &_inds, &_configurator);

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		_nkBuffer.Unbind();

		nk_buffer_free(&_verts);
		nk_buffer_free(&_inds);

		/*
			Made using the examples in the nuklear repository. Made a rough API for nuklear to use with
			my Renderer class.
		*/

		const nk_draw_command* cmd = nullptr;
		uint32_t offset = 0u;

		nk_draw_foreach(cmd, _ctx.get(), &_cmds)
		{
			if (cmd->elem_count == 0u) 
			{
			    continue;
			}

			DrawCall draw{ _nkBuffer.VAO(), GL_TRIANGLES, offset, static_cast<int>(cmd->elem_count) };

			if (cmd->texture.id != 0 && glIsTexture((unsigned int)cmd->texture.id) == GL_TRUE)
			{
				draw.texture = Texture2D{ static_cast<Texture2D::IDType>(cmd->texture.id) };
			}

			_drawCalls.emplace_back(draw);
			offset += (cmd->elem_count * sizeof(uint32_t));
		}

		nk_buffer_clear(&_cmds);
		nk_clear(_ctx.get());

		return std::span<DrawCall> { _drawCalls.begin(), _drawCalls.size() };
	}

	void UIContainer::Update([[maybe_unused]] float dt)
	{
		if(!IsLua()) { return; }
		_dimensions.set("wWidth", _window->GetWidth(), "wHeight", _window->GetHeight());
		
		for (auto& [name, errorMsg] : _luaFunctions)
		{
			const sol::safe_function_result result = (*_lua)[name]();

			if (!result.valid())
			{
				const sol::error err = result;
				std::fprintf(stderr, "%s\n%s\n", errorMsg.c_str(), err.what());
			}
		}
	}

	void UIContainer::InitLua(gsl::not_null<sol::state*> ptr)
	{
		_lua = ptr;	
	
		_lua->open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);

		// Create a reference table for the nuklear functions.
		_dimensions = _lua->create_named_table("Host");
		
		// Useful types
		{

			auto vec2 = _lua->new_usertype<struct nk_vec2>("vec2", sol::no_constructor);
			vec2["x"] = &nk_vec2::x;
			vec2["y"] = &nk_vec2::y;
			(*_lua)["new_vec2"] = nk_vec2;

			auto vec2i = _lua->new_usertype<struct nk_vec2i>("vec2i", sol::no_constructor);
			vec2i["x"] = &nk_vec2i::x;
			vec2i["y"] = &nk_vec2i::y;
			(*_lua)["new_vec2i"] = nk_vec2i;

			auto rect = _lua->new_usertype<struct nk_rect>("rect", sol::no_constructor);
			rect["x"] = &nk_rect::x;
			rect["y"] = &nk_rect::y;
			rect["w"] = &nk_rect::w;
			rect["h"] = &nk_rect::h;
			(*_lua)["new_rect"] = nk_rect;

			auto recti = _lua->new_usertype<struct nk_recti>("recti", sol::no_constructor);
			recti["x"] = &nk_recti::x;
			recti["y"] = &nk_recti::y;
			recti["w"] = &nk_recti::w;
			recti["h"] = &nk_recti::h;
			(*_lua)["new_recti"] = nk_recti;

			auto color = _lua->new_usertype<struct nk_color>("color", sol::no_constructor);
			color["r"] = &nk_color::r;
			color["g"] = &nk_color::g;
			color["b"] = &nk_color::b;
			color["a"] = &nk_color::a;
			(*_lua)["rgba"] = nk_rgba;

			auto colorf = _lua->new_usertype<struct nk_colorf>("colorf", sol::no_constructor);
			colorf["r"] = &nk_colorf::r;
			colorf["g"] = &nk_colorf::g;
			colorf["b"] = &nk_colorf::b;
			colorf["a"] = &nk_colorf::a;
			(*_lua)["rgba_f"] = nk_rgba_f;

			auto scroll = _lua->new_usertype<struct nk_scroll>("scroll", sol::no_constructor);
			scroll["x"] = &nk_scroll::x;
			scroll["y"] = &nk_scroll::y;


			_lua->new_enum<FontStyle>( "FontStyle",
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

			_lua->new_enum<nk_flags>( "TextAlign",
				{
					std::make_pair("Left", NK_TEXT_LEFT),
					std::make_pair("Center", NK_TEXT_CENTERED),
					std::make_pair("Right", NK_TEXT_RIGHT)
				}
				
			);

			_lua->new_enum<nk_panel_flags>( "PanelFlag",
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


			_lua->new_enum<nk_layout_format>("Layout",
				{
					std::make_pair("Dynamic", NK_DYNAMIC),
					std::make_pair("Static", NK_STATIC)
				}
			);

			_lua->new_enum<nk_color_format>("ColorFmt",
				{
					std::make_pair("RGB", NK_RGB),
					std::make_pair("RGBA", NK_RGBA)
				}
			);

			_lua->new_enum<nk_symbol_type>("Symbol",
				{
					std::make_pair("None", NK_SYMBOL_NONE),
					std::make_pair("X", NK_SYMBOL_X),
					std::make_pair("UnderScore", NK_SYMBOL_UNDERSCORE),
					std::make_pair("SolidCircle", NK_SYMBOL_CIRCLE_SOLID),
					std::make_pair("LineCircle", NK_SYMBOL_CIRCLE_OUTLINE),
					std::make_pair("SolidRect", NK_SYMBOL_RECT_SOLID),
					std::make_pair("LineRect", NK_SYMBOL_RECT_OUTLINE),
					std::make_pair("UpTriangle", NK_SYMBOL_TRIANGLE_UP),
					std::make_pair("DownTriangle", NK_SYMBOL_TRIANGLE_DOWN),
					std::make_pair("LeftTriangle", NK_SYMBOL_TRIANGLE_LEFT),
					std::make_pair("RightTriangle", NK_SYMBOL_TRIANGLE_RIGHT),
					std::make_pair("Plus", NK_SYMBOL_PLUS),
					std::make_pair("Minus", NK_SYMBOL_MINUS),
					std::make_pair("Max", NK_SYMBOL_MAX)
				}
			);

			_lua->new_enum<nk_popup_type>("Popup",
				{
					std::make_pair("Static", NK_POPUP_STATIC),
					std::make_pair("Dynamic", NK_POPUP_DYNAMIC),
				}
			);

		}


		// Access to the nuklear context.

		auto context = _lua->new_usertype<struct nk_context>("Context");
		(*_lua)["Ctx"] = _ctx.get();

		/*
			Define nuklear functions.
		*/

		// Windows

		_lua->new_usertype<struct nk_window>("Window");

		context["Begin"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<struct nk_rect> size, sol::optional<nk_panel_flags> flags) -> bool
			{
				// Unfortunately, for now there is no getting around using nk_strlen here.

				return static_cast<bool>(nk_begin(*ctx, text.value().data(), *size, *flags));
			};

		context["End"] = nk_end;

		// Groups

		context["GroupBegin"] =
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				return static_cast<bool>(nk_group_begin(*ctx, text.value().data(), *flags));
			};

		context["GroupEnd"] = nk_group_end;

		context["GroupBeginScroll"] =
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_scroll*> off, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				return static_cast<bool>(nk_group_scrolled_begin(*ctx, *off, text.value().data(), *flags));
			};

		context["GroupEndScroll"] = nk_group_scrolled_end;

		context["GroupGetScroll"] =
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> id) -> std::pair<uint32_t, uint32_t>
			{
				uint32_t scrX = 0, scrY = 0;

				nk_group_get_scroll(*ctx, id.value().data(), &scrX, &scrY);

				return std::make_pair(scrX, scrY);
			};

		context["GroupSetScroll"] =
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> id, sol::optional<uint32_t> offX, sol::optional<uint32_t> offY)
			{
				nk_group_set_scroll(*ctx, id.value().data(), *offX, *offY);
			};

		// Layouts

		context["SpaceRowBegin"] = nk_layout_space_begin;
		context["SpaceRowEnd"] = nk_layout_space_end;
		context["SpaceRowPush"] = nk_layout_space_push;
		context["StaticRow"] = nk_layout_row_static;
		context["DynamicRow"] = nk_layout_row_dynamic;

		//Widgets

		context["MenubarBegin"] = nk_menubar_begin;
		context["MenubarEnd"] = nk_menubar_end;

		context["MenuBeginLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags, sol::optional<struct nk_vec2> size) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_menu_begin_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *size));
			};

		context["MenuBeginImg"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> id, sol::optional<struct nk_vec2> size) -> bool
			{
				/*
					Fix the magic number in this function before testing.
				*/
				return static_cast<bool>(nk_menu_begin_image(*ctx, id.value().data(), nk_image_id(1), *size));
			};

		context["MenuBeginImgLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags, sol::optional<struct nk_image> img, sol::optional<struct nk_vec2> size) -> bool
			{
				const auto& str = text.value();
				
				return static_cast<bool>(nk_menu_begin_image_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *img, *size));

			};

		context["MenuBeginSym"] =
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> id, sol::optional<nk_symbol_type> sym, sol::optional<struct nk_vec2> size) -> bool
			{
				return static_cast<bool>(nk_menu_begin_symbol(*ctx, id.value().data(), *sym, *size));
			}; 

		context["MenuBeginSymLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags, sol::optional<nk_symbol_type> sym, sol::optional<struct nk_vec2> size) -> bool
			{
				const auto& str = text.value();
				
				return static_cast<bool>(nk_menu_begin_symbol_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *sym, *size));
			};

		context["MenuItemLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_menu_item_text(*ctx, str.data(), static_cast<int>(str.size()), *flags));
			};

		context["MenuItemImgLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_image> img, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_menu_item_image_text(*ctx, *img, str.data(), static_cast<int>(str.size()), *flags));

			};

		context["MenuItemSymLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_symbol_type> sym, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_menu_item_symbol_text(*ctx, *sym, str.data(), static_cast<int>(str.size()), *flags));
			};

		context["MenuClose"] = nk_menu_close;
		context["MenuEnd"] = nk_menu_end;

		context["Label"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags)
			{
				const auto& str = text.value();
				
				nk_text(*ctx, str.data(), static_cast<int>(str.size()), *flags);
			};

		context["ButtonLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_button_text(*ctx, str.data(), static_cast<int>(str.size())));
			};

		context["ButtonC"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_color> color) -> bool
			{
				return static_cast<bool>(nk_button_color(*ctx, *color));
			};

		context["ButtonSym"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_symbol_type> sym) -> bool
			{
				return static_cast<bool>(nk_button_symbol(*ctx, *sym));
			};

		context["ButtonImg"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_image> img) -> bool
			{
				return static_cast<bool>(nk_button_image(*ctx, *img));
			};

		context["ButtonSymLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_symbol_type> sym, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				
				const auto& str = text.value();
				
				return static_cast<bool>(nk_button_symbol_text(*ctx, *sym, str.data(), static_cast<int>(str.size()), *flags));
			};

		context["ButtonImgLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_image> img, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{

				const auto& str = text.value();

				return static_cast<bool>(nk_button_image_text(*ctx, *img, str.data(), static_cast<int>(str.size()), *flags));
			};

		//context["ButtonLblSty"] = nk_button_label_styled;
		//context["ButtonSymSty"] = nk_button_symbol_styled;
		//context["ButtonImgSty"] = nk_button_image_styled;
		//context["ButtonSymLblSty"] = nk_button_symbol_label_styled;
		//context["ButtonImgLblSty"] = nk_button_image_label_styled;

		context["CheckLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<bool> active) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_check_text(*ctx, str.data(), static_cast<int>(str.size()), nk_bool{*active}));
			};

		context["CheckFlagLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<unsigned int> flags, sol::optional<unsigned int> value) -> unsigned int
			{
				const auto& str = text.value();

				return nk_check_flags_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *value);
			};

		context["CheckboxLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<int*> active) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_checkbox_text(*ctx, str.data(), static_cast<int>(str.size()), *active));
			};

		context["CheckboxFlagLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<unsigned int*> flags, sol::optional<unsigned int> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_checkbox_flags_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["RadioLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<int*> active) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_radio_text(*ctx, str.data(), static_cast<int>(str.size()), *active));
			};

		context["RadioOptLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<int> active) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_option_text(*ctx, str.data(), static_cast<int>(str.size()), *active));
			};

		context["SelectableLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<int*> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_selectable_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["SelectableImgLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_image> img, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<int*> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_selectable_image_text(*ctx, *img, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["SelectableSymLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_symbol_type> sym, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<int*> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_selectable_symbol_text(*ctx, *sym, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["SelectLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<int> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_select_text(*ctx, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["SelectImgLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_image> img, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<int> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_select_image_text(*ctx, *img, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["SelectSymLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_symbol_type> sym, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<int> value) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_select_symbol_text(*ctx, *sym, str.data(), static_cast<int>(str.size()), *flags, *value));
			};

		context["SlideF"] = nk_slide_float;
		context["SlideI"] = nk_slide_int;

		context["SliderF"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<float> min, sol::optional<float*> value, sol::optional<float> max, sol::optional<float> step) -> bool
			{
				return static_cast<bool>(nk_slider_float(*ctx, *min, *value, *max, *step));
			};

		context["SliderI"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<int> min, sol::optional<int*> value, sol::optional<int> max, sol::optional<int> step) -> bool
			{
				return static_cast<bool>(nk_slider_int(*ctx, *min, *value, *max, *step));
			};

		context["Progress"] =
			[](sol::optional<nk_context*> ctx, sol::optional<uintptr_t*> current, sol::optional<uintptr_t> max, sol::optional<bool> mod) -> bool
			{
				return static_cast<bool>(nk_progress(*ctx, *current, *max, nk_bool{*mod}));
			};
			
		context["Prog"] = nk_prog;

		context["ColorPicker"] = nk_color_picker;

		context["PickColor"] =
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_colorf*> color, sol::optional<nk_color_format> fmt) -> bool
			{
				return static_cast<bool>(nk_color_pick(*ctx, *color, *fmt));
			};

		context["PopupBegin"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_popup_type> type, sol::optional<std::string_view> text, sol::optional<nk_flags> flags, sol::optional<struct nk_rect> bounds) -> bool
			{
				return static_cast<bool>(nk_popup_begin(*ctx, *type, text.value().data(), *flags, *bounds));
			};

		context["PopupClose"] = nk_popup_close;
		context["PopupEnd"] = nk_popup_end;
		context["PopupGetScr"] = nk_popup_get_scroll;
		context["PopupSetScr"] = nk_popup_set_scroll;

		context["Combo"] = nk_combo;
		context["ComboSep"] = nk_combo_separator;
		context["ComboStr"] = nk_combo_string;
		context["ComboCallb"] = nk_combo_callback;
		context["Combobox"] = nk_combobox;
		context["ComboboxStr"] = nk_combobox_string;
		context["ComboboxSep"] = nk_combobox_separator;
		context["ComboboxCallb"] = nk_combobox_callback;

		context["ContextBegin"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_flags> flags, sol::optional<struct nk_vec2> size, sol::optional<struct nk_rect> bounds) -> bool
			{
				return static_cast<bool>(nk_contextual_begin(*ctx, *flags, *size, *bounds));
			};

		context["ContextItemLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				const auto& str = text.value();
				
				return static_cast<bool>(nk_contextual_item_text(*ctx, str.data(), static_cast<int>(str.size()), *flags));
			};

		context["ContextItemImgLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<struct nk_image> img, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_contextual_item_image_text(*ctx, *img, str.data(), static_cast<int>(str.size()), *flags));
			};

		context["ContextItemSymLbl"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<nk_symbol_type> sym, sol::optional<std::string_view> text, sol::optional<nk_panel_flags> flags) -> bool
			{
				const auto& str = text.value();

				return static_cast<bool>(nk_contextual_item_symbol_text(*ctx, *sym, str.data(), static_cast<int>(str.size()), *flags));
			};

		context["ContextClose"] = nk_contextual_close;
		context["ContextEnd"] = nk_contextual_end;

		context["TooltipTxt"] = nk_tooltip;

		context["TooltipBegin"] = 
			[](sol::optional<nk_context*> ctx, sol::optional<float> width) -> bool
			{
				return static_cast<bool>(nk_tooltip_begin(*ctx, *width));
			};

		context["TooltipEnd"] = nk_tooltip_end;

		// Styles

		context["StylePushFont"] = [this](sol::optional<FontStyle> style) -> bool {
				if (style)
				{
					const auto* host = Mapper::GetInstance();
					return static_cast<bool>(nk_style_push_font(_ctx.get(), &host->GetFont(*style)->handle));
				}

				return false;
		};

		context["StylePopFont"] = nk_style_pop_font;

		// Window commands

		context["RequestWindowToClose"] = [this]() { glfwSetWindowShouldClose(_window->GetPtr(), 1); };

		context["RequestWindowToggle"] = [this]() {
			
			if (Mapper::GetInstance()->IsFullscreen())
			{
				glfwRestoreWindow(_window->GetPtr());
			}
			else
			{
				glfwMaximizeWindow(_window->GetPtr());
			}
			
		};

		context["RequestWindowRestore"] = [this]() { glfwRestoreWindow(_window->GetPtr()); };

		context["RequestWindowIconify"] = [this]() { glfwIconifyWindow(_window->GetPtr()); };

	}	
}
