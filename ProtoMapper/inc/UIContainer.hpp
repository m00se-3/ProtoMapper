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

#ifndef PROTOMAPPER_UICONTAINER_HPP
#define PROTOMAPPER_UICONTAINER_HPP

#include "ResourceManager.hpp"
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

namespace proto
{

	/*
		Root UI Frame
	*/
	class RootFrame : public Gwk::Controls::DockBase
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

	class ProtoResourcePaths : public Gwk::ResourcePaths
	{
		std::string _path;

	public:
		ProtoResourcePaths(const std::string& resourcePath);
		virtual ~ProtoResourcePaths() = default;

		std::string GetPath(Gwk::ResourcePaths::Type type, std::string const& relPath) final;

	};

	class UIContainer
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
}
#endif // !PROTOMAPPER_UICONTAINER_HPP
