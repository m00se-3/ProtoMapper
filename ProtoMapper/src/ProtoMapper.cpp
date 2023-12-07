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

#include <chrono>
#include <filesystem>
#include <memory>
#include <span>
#include <unordered_map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "SimpleIni.h"

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_FIXED_TYPES

// We have to explicitly undefine NK_IMPLEMENTATION here to avoid linking errors.
#undef NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

module proto.Mapper;

import proto.UI.Container;
import proto.Scene;
import proto.Renderer;
import proto.ResourceManager;
import proto.Logger;
import proto.Window;

namespace proto 
{
	constexpr const size_t InitialTextBufferSize = 8u * 1024u;	// Allocate 8 KB for the text memory buffer. Can change later if needed.

	Mapper* Mapper::_self = nullptr;

	Mapper* Mapper::GetInstance() { return _self; }

	void Mapper::KeyboardEventCallback([[maybe_unused]] GLFWwindow* window, int keyn, int scancode, int action, int mods)
	{
		auto* self = Mapper::GetInstance();
		int key = Mapper::GLFWKeytoNKKey(keyn, mods);

		if (key > -1)
		{
			nk_input_key(self->UI()->Context(), (nk_keys)key, (action == GLFW_PRESS || action == GLFW_REPEAT));
		}
	}

	void Mapper::TextEventCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint)
	{
		auto* self = Mapper::GetInstance();

		nk_input_unicode(self->UI()->Context(), codepoint);
	}

	void Mapper::MouseButtonEventCallback([[maybe_unused]] GLFWwindow* window, int button, int action, int mods)
	{
		auto* self = Mapper::GetInstance();
		int result = Mapper::GLFWButtontoNKButton(button);

		double mx = 0.0, my = 0.0;
		glfwGetCursorPos(window, &mx, &my);

		if (result > -1)
		{
			nk_input_button(self->UI()->Context(), (nk_buttons)result, (int)std::floor(mx), (int)std::floor(my), (action == GLFW_PRESS));
		}
	}

	void Mapper::MouseMotionEventCallback([[maybe_unused]]GLFWwindow*, double x, double y)
	{
		auto* self = Mapper::GetInstance();

		nk_input_motion(self->UI()->Context(), (int)std::floor(x), (int)std::floor(y));
	}

	void Mapper::MouseScrollEventCallback([[maybe_unused]] GLFWwindow* window, double offX, double offY)
	{
		auto* self = Mapper::GetInstance();

		nk_input_scroll(self->UI()->Context(), nk_vec2((float)offX, (float)offY));
	}

	void Mapper::DropEventCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]]int count, [[maybe_unused]] const char** paths)
	{
	}

	void Mapper::FrameBufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height)
	{
		// If the window was minimized, do nothing.
		if(width == 0 || height == 0)
		{
			return;
		}
		
		auto* self = Mapper::GetInstance();

		int oW = self->GetWin().GetWidth(),
			oH = self->GetWin().GetHeight(),
			rX = self->GetRenderer()->GetRenderX(),
			rY = self->GetRenderer()->GetRenderY(),
			rW = self->GetRenderer()->GetRenderWidth(),
			rH = self->GetRenderer()->GetRenderHeight();

		float sW = ((float)width / (float)oW);
		float sH = ((float)height / (float)oH);

		self->SetContextSize(width, height);
		self->GetRenderer()->RefreshProjection();

		const int nx = std::lround((float)rX * sW);
		const int ny = std::lround((float)rY * sH);
		const int nw = std::lround((float)rW * sW);
		const int nh = std::lround((float)rH * sH);

		self->GetRenderer()->SetViewport(nx, ny, nw, nh);

	}

	Mapper::~Mapper()
	{
		if (_configUpdate)
		{
			_configData.SaveFile(_configFile.c_str());
		}

		_configData.Reset();
		_scene->Cleanup();
		_scene.reset(nullptr);
		
		Logger::Reset();

		_ui.reset(nullptr);
		_renderer.reset(nullptr);
		_resources.reset(nullptr);
	}

	bool Mapper::Configure()
	{
		_self = this;

		if (!glfwInit())
		{
			return false;
		}


#if defined(_DEBUG_) || defined(_RELWDEBUGSYM_)

		_rootDir = ROOT_DIR;
		_configFile = _rootDir.string() + "/config/config.ini";

#else
		_rootDir = std::filesystem::current_path();
		_configFile = _rootDir.string() + "/config/config.ini";

#endif // _DEBUG_

		/*
			Load the config.ini file.
		*/

		if (std::filesystem::exists(_configFile))
		{
			auto err = _configData.LoadFile(_configFile.string().c_str());

			if (err < 0) return false;
		}
		else
			return false;

		/*
			Read and set the map of data file directories. This will be used to reference all configuration files
			and assets.
		*/

		std::list<CSimpleIniA::Entry> preloadDirectoryKeys;
		if (!_configData.GetAllKeys("preload_directories", preloadDirectoryKeys)) return false;

		auto tmpRoot = _rootDir.string();

		for (auto& key : preloadDirectoryKeys)
		{
			auto item = _configData.GetValue("preload_directories", key.pItem);

			if (item != nullptr)
			{
				_dataTextFields.insert_or_assign(key.pItem, (tmpRoot + item).c_str());
			}
		}

		preloadDirectoryKeys.clear();

		/*
			Read and set the window dimensions.
		*/

		auto width = _configData.GetLongValue("startup_display", "width", -1);
		auto height = _configData.GetLongValue("startup_display", "height", -1);

		if (width < 0 || height < 0)
			return false;

		/*
			If one of the dimensions is zero, it's safe to assume the native display resolution is being used.
		*/
		if (width != 0 && height != 0)
		{
			_window.SetSize(width, height);
			_fullscreen = false;
		}

		/*
			Setup the ResourceManager.
		*/

		_stringMemoryBuffer = std::make_unique<uint8_t[]>(InitialTextBufferSize);

		_resources = std::make_unique<ResourceManager>(std::span<uint8_t>{_stringMemoryBuffer.get(), InitialTextBufferSize});
		Renderer::SetResourceManager(_resources.get());
		UIContainer::SetResourceManager(_resources->Textures());

		return true;
	}


	int Mapper::Run()
	{
		using time = std::chrono::high_resolution_clock;

		if(!_window.Construct(_title, _fullscreen)) return 2;

		/*
			Create internal Renderer, UIContainer, and Scene objects.
		*/

		_renderer = std::make_unique<Renderer>(_rootDir.string().c_str());
		_renderer->SetRenderSize(_window.GetWidth(), _window.GetHeight());
		_renderer->SetViewport(0, 0, _window.GetWidth(), _window.GetHeight());
		_renderer->Init(Renderer::mode::Two);

		_ui = std::make_unique<UIContainer>(_dataTextFields.at("assets_dir"));

		Window::SetUIHandle(_ui.get());
		Logger::Init();

		glfwSetKeyCallback(_window.GetPtr(), Mapper::KeyboardEventCallback);
		glfwSetCharCallback(_window.GetPtr(), Mapper::TextEventCallback);
		glfwSetMouseButtonCallback(_window.GetPtr(), Mapper::MouseButtonEventCallback);
		glfwSetCursorPosCallback(_window.GetPtr(), Mapper::MouseMotionEventCallback);
		glfwSetScrollCallback(_window.GetPtr(), Mapper::MouseScrollEventCallback);
		glfwSetDropCallback(_window.GetPtr(), Mapper::DropEventCallback);
		glfwSetFramebufferSizeCallback(_window.GetPtr(), Mapper::FrameBufferSizeCallback);

		_scene = std::make_unique<Scene>(_ui.get());
		_scene->Init();

		/*
			TODO: Load all assets on multiple threads.
		*/

		/*
			Load all preload assets and data files. 
		*/
		if(!_ui->SetDefinitionsPath(_dataTextFields.at("user_interface_dir"))) return 3;

		// Text files
		std::filesystem::path textDir = _dataTextFields.at("text_dir");

		for(const auto& entry : std::filesystem::recursive_directory_iterator(textDir))
		{
			if(entry.is_regular_file())
			{

			}
		}

		

		
		/*
			Show main window and start main loop.
		*/

		glfwShowWindow(_window.GetPtr());
		glClearColor(0.3f, 0.3f, 0.3f, 1.f);

		time::time_point last = time::now();

		while (!glfwWindowShouldClose(_window.GetPtr()) && _appRunning)
		{
			time::time_point current = time::now();
			float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());

			_scene->Update(microseconds * 1000000.f);
			
			_ui->Update((float)_window.GetWidth(), (float)_window.GetHeight());
			_ui->Compile();

			_renderer->Begin();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_scene->DrawNodes();

			_ui->Draw(_renderer.get());

			glfwSwapBuffers(_window.GetPtr());
			_renderer->End();

			/*
				Capture input events for the GUI and the simulation.
			*/
			nk_input_begin(_ui->Context());
			glfwPollEvents();
			nk_input_end(_ui->Context());
		}

		return EXIT_SUCCESS;

	}

	void Mapper::SetContextSize(int w, int h)
	{
		_window.SetSize(w, h);
		_renderer->SetRenderSize(w, h);
	}

	Window& Mapper::GetWin() { return _window; }

	Renderer* Mapper::GetRenderer()
	{
		return _renderer.get();
	}

	UIContainer* Mapper::UI()
	{
		return _ui.get();
	}

	int Mapper::GLFWKeytoNKKey(int key, int mods)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT_SHIFT:			return NK_KEY_SHIFT;
		case GLFW_KEY_RIGHT_SHIFT:			return NK_KEY_SHIFT;
		case GLFW_KEY_LEFT_CONTROL:			return NK_KEY_CTRL;
		case GLFW_KEY_RIGHT_CONTROL:		return NK_KEY_CTRL;
		case GLFW_KEY_DELETE:				return NK_KEY_DEL;
		case GLFW_KEY_ENTER:				return NK_KEY_ENTER;
		case GLFW_KEY_TAB:					return NK_KEY_TAB;
		case GLFW_KEY_BACKSPACE:			return NK_KEY_BACKSPACE;
		case GLFW_KEY_UP:					return NK_KEY_UP;
		case GLFW_KEY_DOWN:					return NK_KEY_DOWN;
		case GLFW_KEY_LEFT:					return NK_KEY_LEFT;
		case GLFW_KEY_RIGHT:				return NK_KEY_RIGHT;
		case GLFW_KEY_C:
		{
			if (mods & GLFW_MOD_CONTROL)
				return NK_KEY_COPY;
			
			break;
		}
		case GLFW_KEY_V:
		{
			if (mods & GLFW_MOD_CONTROL)
				return NK_KEY_PASTE;

			break;
		}
		case GLFW_KEY_X:
		{
			if (mods & GLFW_MOD_CONTROL)
				return NK_KEY_CUT;

			break;
		}
		default:							return -1;			// Not a nk_key.
		}

		return -1;			// Not a nk_key.
	}

	int Mapper::GLFWButtontoNKButton(int button)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:			return NK_BUTTON_LEFT;
		case GLFW_MOUSE_BUTTON_RIGHT:			return NK_BUTTON_RIGHT;
		case GLFW_MOUSE_BUTTON_MIDDLE:			return NK_BUTTON_MIDDLE;
		default:								return -1;					// Not an nk_button.
		}
	}
}



