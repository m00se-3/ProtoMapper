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
#ifndef PROTO_MAPPER_HPP
#define PROTO_MAPPER_HPP

#include <filesystem>
#include <memory>
#include <unordered_map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "SimpleIni.h"

#include "UIContainer.hpp"
#include "Scene.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Logger.hpp"
#include "Window.hpp"

namespace proto
{
    class Mapper
	{
	public:
		Mapper() = default;
		~Mapper();

		[[nodiscard]]static Mapper* GetInstance();
		[[nodiscard]]bool Configure();
		[[nodiscard]]int Run();
		void SetContextSize(int w, int h);

		[[nodiscard]]Window& GetWin();
		[[nodiscard]]Renderer* GetRenderer();
		[[nodiscard]]UIContainer* UI();

		// GLFW input event callbacks.
		
		static void KeyboardEventCallback(GLFWwindow* window, int keyn, int scancode, int action, int mods);
		static void TextEventCallback(GLFWwindow* window, unsigned int codepoint);
		static void MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods);
		static void MouseMotionEventCallback(GLFWwindow*, double x, double y);
		static void MouseScrollEventCallback(GLFWwindow* window, double offX, double offY);

		static int GLFWKeytoNKKey(int key, int mods);
		static int GLFWButtontoNKButton(int button);

	private:
		const std::string _title = "ProtoMapper";
		std::filesystem::path _rootDir;
		bool _appRunning = true, _fullscreen = true, _configUpdate = false;

		std::unique_ptr<uint8_t[]> _stringMemoryBuffer;

		std::unique_ptr<Scene> _scene;
		std::unique_ptr<UIContainer> _ui;
		std::unique_ptr<Renderer> _renderer;
		std::unique_ptr<ResourceManager> _resources;

		std::filesystem::path _configFile;
		CSimpleIniA _configData;
		Window _window;

		// Text directories as defined in config.ini section [preload_directories].
		std::unordered_map<std::string, std::string> _dataTextFields;

		static Mapper* _self;

	};
}

#endif