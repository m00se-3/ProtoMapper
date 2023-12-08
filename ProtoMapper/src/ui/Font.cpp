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
#include "Font.hpp"

namespace proto
{
    FontGroup::FontGroup()
    {
        nk_font_atlas_init_default(&_atlas);
    }

    FontGroup::~FontGroup()
    {
        nk_font_atlas_cleanup(&_atlas);
		nk_font_atlas_clear(&_atlas);
    }

    void FontGroup::Create()
    {
        nk_font_atlas_begin(&_atlas);
    }

    void FontGroup::Finalize(unsigned int id)
    {
        nk_font_atlas_end(&_atlas, nk_handle_id((int)id), nullptr);
    }
    
    void FontGroup::AddFont(FontStyle styleMask, float size, const std::filesystem::path& filename)
    {
        if(!_fonts.contains(styleMask))
        {
            struct nk_font* temp = nk_font_atlas_add_from_file(&_atlas, filename.string().c_str(), size, nullptr);
            if(temp) _fonts.insert_or_assign(styleMask, temp);
        }
    }

    nk_font* FontGroup::GetFont(FontStyle mask)
    {
        if(_fonts.contains(mask))
        {
            return _fonts.at(mask);
        }

        return nullptr;
    }

    const nk_font* FontGroup::GetFont(FontStyle mask) const
    {
        if(_fonts.contains(mask))
        {
            return _fonts.at(mask);
        }

        return nullptr;
    }

    [[nodiscard]]nk_font_atlas* FontGroup::GetAtlas() { return &_atlas; }

    [[nodiscard]]const nk_font_atlas* FontGroup::GetAtlas() const { return &_atlas; }
}