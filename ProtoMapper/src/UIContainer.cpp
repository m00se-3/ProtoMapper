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

#include "Gwork/Controls/Canvas.h"
#include "Gwork/Gwork.h"
#include "Gwork/Renderers/OpenGLCore.h"
#include "Gwork/Platform.h"
#include "Gwork/Input/GLFW3.h"
#include "Gwork/Skins/Simple.h"
#include "Gwork/Skins/TexturedBase.h"
#include "SimpleIni.h"

export module proto.UI.Container;

import UI.Root;
import UI.LogFrame;

namespace proto
{
	export class ProtoResourcePaths;
	
	/*
		UIContainer definitions.
	*/

	export class UIContainer
	{
	public:
		UIContainer(Gwk::ResourcePaths& paths, int width, int height);
		~UIContainer();

		[[nodiscard]]bool SetDefinitionsPath(const std::filesystem::path& filepath);
		[[nodiscard]]bool ConstructWithProfile(const std::filesystem::path& filepath);
		[[nodiscard]]Gwk::Input::GLFW3* InputHandle();
		[[nodiscard]]std::shared_ptr<LogFrame> GetLogUI();

		void AddFont(const std::filesystem::path& filepath);
		void Draw();
		void SetSize(int width, int height);

	private:
		std::filesystem::path _interfaceDir;
		
		/*
			Gwork stuff.
		*/
		Gwk::Input::GLFW3 _inputHandle;
		std::unique_ptr<Gwk::Renderer::OpenGLCore> _renderer;
		std::unique_ptr<Gwk::Skin::TexturedBase> _skin;
		std::unique_ptr<Gwk::Controls::Canvas> _canvas;
		std::unique_ptr<RootFrame> _frame;

	};


	UIContainer::UIContainer(Gwk::ResourcePaths& paths, int width, int height)
		: _renderer(std::make_unique<Gwk::Renderer::OpenGLCore>(paths, Gwk::Rect(Gwk::Point(0,0), Gwk::Point(width,height))))
	{
		// Finish setup.
		_renderer->Init();
		_skin = std::make_unique<Gwk::Skin::TexturedBase>(_renderer.get());
		_skin->Init("DefaultSkin.png");
		_skin->SetDefaultFont("OpenSans.ttf", 12.f);
		
		_canvas = std::make_unique<Gwk::Controls::Canvas>(_skin.get());
		_canvas->SetSize(Gwk::Point(width, height));

		_inputHandle.Initialize(_canvas.get());
		_frame = std::make_unique<RootFrame>(_canvas.get());
	}

	UIContainer::~UIContainer()
	{
		_frame.reset(nullptr);
		_canvas.reset(nullptr);
		_skin.reset(nullptr);
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

	bool UIContainer::ConstructWithProfile(const std::filesystem::path& filepath)
	{
		return _frame->Construct(filepath);
	}

    void UIContainer::AddFont(const std::filesystem::path& filepath)
    {
	}

	Gwk::Input::GLFW3* UIContainer::InputHandle() { return &_inputHandle; }

	std::shared_ptr<LogFrame> UIContainer::GetLogUI() { return _frame->GetLogUI(); }

	void UIContainer::Draw()
	{
		_canvas->RenderCanvas();
	}

	void UIContainer::SetSize(int width, int height)
	{
		_canvas->SetSize(width, height);
	}
}
