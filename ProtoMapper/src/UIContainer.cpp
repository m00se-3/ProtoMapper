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

#include "Gwork/Controls/Canvas.h"
#include "SimpleIni.h"
#include "Gwork/Renderers/OpenGLCore.h"
#include "Gwork/Gwork.h"
#include "Gwork/Platform.h"
#include "Gwork/Controls/DockBase.h"
#include "Gwork/Controls/StatusBar.h"
#include "Gwork/Skins/Simple.h"
#include "Gwork/Skins/TexturedBase.h"
#include "Gwork/Input/GLFW3.h"

#include <memory>
#include <map>
#include <string>
#include <filesystem>

export module UIContainer;

import ResourceManager;

export namespace proto
{

	export class RootFrame : public Gwk::Controls::DockBase
	{
		public:
			// Define the constructor and Gwk meta data functions.
			GWK_CONTROL(RootFrame, Gwk::Controls::DockBase);
		
			void Render(Gwk::Skin::Base* skin) override;

			[[nodiscard]]bool Construct(const std::filesystem::path& root);
		private:

			std::unique_ptr<Gwk::Controls::StatusBar> _statusBar;
			std::unique_ptr<Gwk::Controls::Button> _button;
	};

	export class ProtoResourcePaths : public Gwk::ResourcePaths
	{
		std::string _path;

	public:
		ProtoResourcePaths(const std::string& resourcePath);
		virtual ~ProtoResourcePaths() = default;

		std::string GetPath(Gwk::ResourcePaths::Type type, std::string const& relPath) final;

	};

	export class UIContainer
	{
		std::filesystem::path _interfaceDir;
		
		/*
			Gwork stuff.
		*/
		Gwk::Input::GLFW3 _inputHandle;
		std::unique_ptr<Gwk::Renderer::OpenGLCore> _renderer;
		std::unique_ptr<Gwk::Skin::TexturedBase> _skin;
		std::unique_ptr<Gwk::Controls::Canvas> _canvas;
		std::unique_ptr<RootFrame> _frame;


	public:
		UIContainer(Gwk::ResourcePaths& paths, int width, int height);
		~UIContainer();

		[[nodiscard]]bool SetDefaultTexture(const std::filesystem::path& dir);
		[[nodiscard]]bool SetDefaultFont(const std::filesystem::path& dir);
		[[nodiscard]]bool SetDefinitionsPath(const std::filesystem::path& filepath);
		[[nodiscard]]Gwk::Input::GLFW3* InputHandle();

		void AddFont(const std::filesystem::path& filepath);
		void Draw();


		static void SetResourceManager(ResourceManager* ptr);


	private:
		static ResourceManager* _resources;


	};

	ResourceManager* UIContainer::_resources = nullptr;


	void UIContainer::SetResourceManager(ResourceManager* ptr) { _resources = ptr; }

	/*
		RootFrame definitions.
	*/

	GWK_CONTROL_CONSTRUCTOR(RootFrame) // Member initialization is handled by the macro.
	{
		Dock(Gwk::Position::Fill);

		_statusBar = std::make_unique<Gwk::Controls::StatusBar>(this);
		_statusBar->Dock(Gwk::Position::Bottom);
		_statusBar->SetTextColor(Gwk::Color{ 0u, 0u, 0u, 255u });
		_statusBar->SetText("This is working.");

		_button = std::make_unique<Gwk::Controls::Button>(this);
		_button->SetBounds(Gwk::Rect{ 10, 10, 50, 20 });
		_button->SetText("Hello!");
	}

	void RootFrame::Render(Gwk::Skin::Base* skin)
	{
		ParentClass::Render(skin);
	}

	bool RootFrame::Construct(const std::filesystem::path& root)
	{


		return true;
	}

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
			result += "/UIskins/";
		}
		
		return (result + relPath);
	}


	/*
		UIContainer definitions.
	*/


	UIContainer::UIContainer(Gwk::ResourcePaths& paths, int width, int height)
		: _renderer(std::make_unique<Gwk::Renderer::OpenGLCore>(paths, Gwk::Rect(Gwk::Point(0,0), Gwk::Point(width,height))))
	{
		// Finish setup.
		_renderer->Init();
		_skin = std::make_unique<Gwk::Skin::TexturedBase>(_renderer.get());
		_skin->Init("DefaultSkin.png");
		_skin->SetDefaultFont("OpenSans.ttf");
		
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

    void UIContainer::AddFont(const std::filesystem::path& filepath)
    {
	}

	Gwk::Input::GLFW3* UIContainer::InputHandle() { return &_inputHandle; }

	void UIContainer::Draw()
	{
		_canvas->RenderCanvas();
	}
}
