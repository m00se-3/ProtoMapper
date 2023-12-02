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

#include <filesystem>
#include <unordered_map> 

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_FIXED_TYPES

// We have to explicitly undefine NK_IMPLEMENTATION here to avoid linking errors.
#undef NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

export module UI.Font;


namespace proto
{
    export enum class FontStyle : uint32_t
    {
        None = 0u,
        Normal = 1u,
        Bold = 2u,
        Italic = 4u,
        Underlined = 8u,
        BoldItalic = Bold | Italic,
        BoldUnderlinded = Bold | Underlined,
        BolItalicUnderlined = Bold | Italic | Underlined,
        ItalicUnderlined = Italic | Underlined,
    };
    
    export class FontGroup
    {
    public:
        FontGroup() = default;
        ~FontGroup() = default;

        void AddFont(struct nk_font_atlas* atlas, FontStyle styleMask, float size, const std::filesystem::path& filename);
        [[nodiscard]]nk_font* GetFont(FontStyle mask);
        [[nodiscard]]const nk_font* GetFont(FontStyle mask) const;

    private:
        std::unordered_map<FontStyle, struct nk_font*> _fonts;
    };
}