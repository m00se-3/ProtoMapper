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
#include "ProtoMapper.hpp"
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
			nk_input_button(self->UI()->Context(), (nk_buttons)result, std::lround(mx), std::lround(my), (action == GLFW_PRESS));
		}
	}

	void Mapper::MouseMotionEventCallback([[maybe_unused]]GLFWwindow*, double x, double y)
	{
		auto* self = Mapper::GetInstance();

		nk_input_motion(self->UI()->Context(), std::lround(x), std::lround(y));
	}

	void Mapper::MouseScrollEventCallback([[maybe_unused]] GLFWwindow* window, double offX, double offY)
	{
		auto* self = Mapper::GetInstance();

		nk_input_scroll(self->UI()->Context(), nk_vec2((float)offX, (float)offY));
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
		
		_ui.reset(nullptr);
		_renderer.reset(nullptr);
		_resources.reset(nullptr);
	}

	bool Mapper::IsFullscreen() const { return _fullscreen; }

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

		glfwSetKeyCallback(_window.GetPtr(), Mapper::KeyboardEventCallback);
		glfwSetCharCallback(_window.GetPtr(), Mapper::TextEventCallback);
		glfwSetMouseButtonCallback(_window.GetPtr(), Mapper::MouseButtonEventCallback);
		glfwSetCursorPosCallback(_window.GetPtr(), Mapper::MouseMotionEventCallback);
		glfwSetScrollCallback(_window.GetPtr(), Mapper::MouseScrollEventCallback);

		_scene = std::make_unique<Scene>();
		_scene->Init();

		/*
			TODO: Load all assets on multiple threads.
		*/

		/*
			Load all preload assets and data files. 
		*/
		if(!_ui->SetDefinitions(_dataTextFields.at("user_interface_dir"))) return 3;

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
		_renderer->SetBackgroundColor(glm::vec4{ 0.3f, 0.3f, 0.3f, 1.f });

		time::time_point last = time::now();

		while (!glfwWindowShouldClose(_window.GetPtr()) && _appRunning)
		{
			time::time_point current = time::now();
			float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());
			
			_ui->Update((float)_window.GetWidth(), (float)_window.GetHeight());
			_ui->Compile();

			_scene->Update(microseconds * 1000000.f);

			_renderer->Begin();

			_scene->Render();
			_ui->Draw(_renderer.get());

			_renderer->End();
			glfwSwapBuffers(_window.GetPtr());

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

	void Mapper::SetFullscreen(bool full)
	{
		_fullscreen = full;
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



