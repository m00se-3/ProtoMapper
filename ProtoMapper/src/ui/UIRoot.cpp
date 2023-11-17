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
#include <string_view>

#include "Gwork/Gwork.h"
#include "Gwork/Controls/DockBase.h"
#include "Gwork/Controls/StatusBar.h"
#include "Gwork/Controls/Canvas.h"
#include "Gwork/Controls/WindowControl.h"


#include "Gwork/Skins/Simple.h"
#include "Gwork/Skins/TexturedBase.h"

#include "SimpleIni.h"

export module UI.Root;

import UI.LogFrame;
import UI.TopMenu;

namespace proto
{
    /*
		Root UI Frame
	*/
	export class RootFrame : public Gwk::Controls::DockBase
	{
		GWK_CONTROL(RootFrame, Gwk::Controls::DockBase);
		
			void Render(Gwk::Skin::Base* skin) override;

			[[nodiscard]]bool Construct(const std::filesystem::path& root);
			[[nodiscard]]std::shared_ptr<LogFrame> GetLogUI();

		private:

			std::unique_ptr<TopMenu> _topMenu;
			std::unique_ptr<Gwk::Controls::StatusBar> _statusBar;
			std::shared_ptr<LogFrame> _logBox;
			
	};

	export class ProtoResourcePaths : public Gwk::ResourcePaths
	{
	public:
		ProtoResourcePaths(const std::string& resourcePath);
		virtual ~ProtoResourcePaths() = default;

		std::string GetPath(Gwk::ResourcePaths::Type type, std::string const& relPath) final;

	private:
		std::string _path;

	};

    /*
		RootFrame definitions.
	*/

	GWK_CONTROL_CONSTRUCTOR(RootFrame)
	{
		Dock(Gwk::Position::Fill);
	}

	void RootFrame::Render(Gwk::Skin::Base* skin)
	{
		ParentClass::Render(skin);
	}

	bool RootFrame::Construct(const std::filesystem::path& root)
	{
		_topMenu = std::make_unique<TopMenu>(this);
		_statusBar = std::make_unique<Gwk::Controls::StatusBar>(this);
		_logBox = std::make_shared<LogFrame>(this);

		_statusBar->Dock(Gwk::Position::Bottom);
		_statusBar->SetTextColor(Gwk::Color{ 0u, 0u, 0u, 255u });
		_statusBar->SetText("What's a good dad joke? You!");

		_logBox->SetHidden(true);

		return true;
	}

	std::shared_ptr<LogFrame> RootFrame::GetLogUI() { return _logBox; }
	

	/*
		ProtoResourcePaths definitions
	*/

	ProtoResourcePaths::ProtoResourcePaths(const std::string& resourcePath)
		: _path(resourcePath)
	{}

	std::string ProtoResourcePaths::GetPath(Gwk::ResourcePaths::Type type, std::string const& relPath)
	{
		std::string result = _path;

		if (type == Gwk::ResourcePaths::Type::Font)
		{
			result += "/fonts/";
		}

		if (type == Gwk::ResourcePaths::Type::Texture)
		{
			if(relPath.starts_with("icon"))
			{
				result += "/icons/";
			}
			else
			{
				result += "/UIskins/";
			}
		}
		
		return (result + relPath);
	}
}