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
    FontAtlas::FontAtlas()
    {
        nk_font_atlas_init_default(_atlas.get());
    }

    FontAtlas::~FontAtlas()
    {
        nk_font_atlas_cleanup(_atlas.get());
	nk_font_atlas_clear(_atlas.get());
    }

    void FontGroup::Create()
    {
        nk_font_atlas_begin(_atlas.Get());
    }

    void FontGroup::Finalize(unsigned int id)
    {
        nk_font_atlas_end(_atlas.Get(), nk_handle_id((int)id), nullptr);
    }
    
    void FontGroup::AddFont(FontStyle styleMask, float size, const std::filesystem::path& filename)
    {
        if(!_fonts.contains(styleMask))
        {
            struct nk_font* temp = nk_font_atlas_add_from_file(_atlas.Get(), filename.string().c_str(), size, nullptr);
            if(temp != nullptr) 
            {
                _fonts.insert_or_assign(styleMask, temp);
            }
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

}
