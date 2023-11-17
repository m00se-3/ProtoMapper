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
#include <unordered_map>
#include <string>
#include <format>


#include "Gwork/Gwork.h"
#include "Gwork/Controls/TabControl.h"
#include "Gwork/Controls/ListBox.h"

export module UI.LogFrame;


namespace proto
{
	/*
        An isolated control that can recieve log messages throughout the application.

        TODO: Setup types for messages, possibly having different colors or prefixes.
    */
    export class LogFrame : public Gwk::Controls::TabControl
    {
	public:
		GWK_CONTROL(LogFrame, Gwk::Controls::TabControl);

		void OutputToTab(const std::string& logTab, const std::string& msg);

	private:

		std::unordered_map<std::string, std::shared_ptr<Gwk::Controls::ListBox>> _logTabs;
    };

    GWK_CONTROL_CONSTRUCTOR(LogFrame)
    {
        Dock(Gwk::Position::Bottom);
		SetSize(GetParent()->Width(), 100 );
		SetKeyboardInputEnabled(true);

		_logTabs["Output"] = std::make_shared<Gwk::Controls::ListBox>(this);

		auto* button = AddPage("Output", _logTabs.at("Output").get());

	}
	
    void LogFrame::OutputToTab(const std::string& logTab, const std::string& msg)
	{
		if(_logTabs.contains(logTab))
		{
			auto tab = _logTabs.at(logTab);

			tab->AddItem(msg);
			tab->ScrollToBottom();
		}
	}

} // namespace proto
