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
#include "Gwork/DragAndDrop.h"
#include "Gwork/Controls/Dragger.h"
#include "Gwork/Controls/Button.h"
#include "Gwork/Controls/Label.h"

#include "Gwork/Skins/Simple.h"
#include "Gwork/Skins/TexturedBase.h"

#include "GLFW/glfw3.h"

export module UI.TitleBar;

namespace proto
{

    export class TitleBar : public Gwk::ControlsInternal::Dragger
    {
    public:
        GWK_CONTROL(TitleBar, Gwk::ControlsInternal::Dragger);

        void SetMaximized(bool m);
        void SetWindowPtr(GLFWwindow* win);

        /*
            Window event handlers.
        */

        void WindowCloseEvent(Gwk::Event::Info);
        void WindowToggleEvent(Gwk::Event::Info);
        void WindowMinimizeEvent(Gwk::Event::Info);

    private:
        std::unique_ptr<Gwk::Controls::Button> _closeBtn;
        std::unique_ptr<Gwk::Controls::Button> _maxBtn;
        std::unique_ptr<Gwk::Controls::Button> _minBtn;
        std::unique_ptr<Gwk::Controls::Label> _title;

        bool _maximized = false;
        GLFWwindow* _window = nullptr;
    };

    GWK_CONTROL_CONSTRUCTOR(TitleBar)
    {
        Dock(Gwk::Position::Top);
        SetHeight(24);
        SetPadding(Gwk::Padding{});
        SetMargin(Gwk::Margin{});
        SetTabable(false);

        /*
            Title label
        */
        _title = std::make_unique<Gwk::Controls::Label>(this);
        _title->Dock(Gwk::Position::Fill);
        _title->SetAlignment(Gwk::Position::Center);
        _title->SetText("ProtoMapper");
        _title->SetTextColor(Gwk::Color{255u, 0u, 0u, 255u});
        _title->SetTabable(false);

        /*
            Close Button
        */

        _closeBtn = std::make_unique<Gwk::Controls::Button>(this, "closeApp");
        _closeBtn->Dock(Gwk::Position::Right);
        _closeBtn->SetText("");
        _closeBtn->SetWidth(24);
        _closeBtn->SetMargin(Gwk::Margin{});
        _closeBtn->SetPadding(Gwk::Padding{});
        _closeBtn->SetImage("iconClose.png", true);
        _closeBtn->onPress.Add(this, &TitleBar::WindowCloseEvent);


        /*
            Maximize Button
        */ 
        _maxBtn = std::make_unique<Gwk::Controls::Button>(this, "toggleApp");
        _maxBtn->Dock(Gwk::Position::Right);
        _maxBtn->SetText("");
        _maxBtn->SetWidth(24);
        _maxBtn->SetMargin(Gwk::Margin{});
        _maxBtn->SetPadding(Gwk::Padding{});
        _maxBtn->SetImage("iconWindowMaximize.png", true);
        _maxBtn->onPress.Add(this, &TitleBar::WindowToggleEvent);


        /*
            Minimize Button
        */ 
        _minBtn = std::make_unique<Gwk::Controls::Button>(this, "minimizeApp");
        _minBtn->Dock(Gwk::Position::Right);
        _minBtn->SetText("");
        _minBtn->SetWidth(24);
        _minBtn->SetMargin(Gwk::Margin{});
        _minBtn->SetPadding(Gwk::Padding{});
        _minBtn->SetImage("iconWindowMinimize.png", true);
        _minBtn->onPress.Add(this, &TitleBar::WindowMinimizeEvent);

    }

    void TitleBar::SetMaximized(bool m) { _maximized = m; }

    void TitleBar::SetWindowPtr(GLFWwindow* win)
	{
		_window = win;
	}

    void TitleBar::WindowCloseEvent(Gwk::Event::Info)
    {
        glfwSetWindowShouldClose(_window, 1);
    }

    void TitleBar::WindowToggleEvent(Gwk::Event::Info)
    {
        if(_maximized)
        {
            _maximized = false;
            _maxBtn->SetImage("iconWindowMaximize.png", true);
            glfwRestoreWindow(_window);
        }
        else
        {
            _maximized = true;
            _maxBtn->SetImage("iconWindowRestore.png", true);
            glfwMaximizeWindow(_window);
        }
    }

    void TitleBar::WindowMinimizeEvent(Gwk::Event::Info)
    {
        glfwIconifyWindow(_window);
    }

}