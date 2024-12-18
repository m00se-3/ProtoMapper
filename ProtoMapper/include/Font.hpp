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
#ifndef PROTO_FONT_HPP
#define PROTO_FONT_HPP

#include <filesystem>
#include <unordered_map> 

extern "C"
{
    struct nk_font;
    struct nk_font_atlas;
}

namespace proto
{
    enum class FontStyle : uint8_t
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

    class FontAtlas 
    {
    public:
        FontAtlas();
        FontAtlas(const FontAtlas &) = default;
        FontAtlas(FontAtlas &&) = delete;
        FontAtlas &operator=(const FontAtlas &) = default;
        FontAtlas &operator=(FontAtlas &&) = delete;
        ~FontAtlas();

        [[nodiscard]] struct nk_font_atlas* Get() const { return _atlas.get(); }

    private:
        std::shared_ptr<struct nk_font_atlas> _atlas;
    };
        
    class FontGroup
    {
    public:
        void Create();
        void Finalize(unsigned int id);
        void AddFont(FontStyle styleMask, float size, const std::filesystem::path& filename);

        [[nodiscard]] nk_font* GetFont(FontStyle mask);
        [[nodiscard]] const nk_font* GetFont(FontStyle mask) const;
        [[nodiscard]] auto* GetAtlas(this auto&& self) { return self._atlas.Get(); }

    private:
        FontAtlas _atlas;
        std::unordered_map<FontStyle, struct nk_font*> _fonts;
    };
}

#endif
