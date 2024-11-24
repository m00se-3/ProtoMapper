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
#include <ProtoMapper.hpp>
#include <Config.hpp>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <sol/stack_core.hpp>

namespace proto 
{
	Mapper* Mapper::_self = nullptr;

	void Mapper::KeyboardEventCallback([[maybe_unused]] GLFWwindow* window, int keyn, [[maybe_unused]] int scancode, int action, int mods)
	{
		auto* self = Mapper::GetInstance();
		const int key = Mapper::GLFWKeytoNKKey(keyn, mods);

		if (key > -1)
		{
			nk_input_key(self->UI()->Context(), (nk_keys)key, nk_bool(action == GLFW_PRESS || action == GLFW_REPEAT));
		}
	}

	void Mapper::TextEventCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint)
	{
		auto* self = Mapper::GetInstance();

		nk_input_unicode(self->UI()->Context(), codepoint);
	}

	void Mapper::MouseButtonEventCallback(GLFWwindow* window, int button, int action, [[maybe_unused]] int mods)
	{
		auto* self = Mapper::GetInstance();
		const int result = Mapper::GLFWButtontoNKButton(button);

		double mx{}, my{};
		glfwGetCursorPos(window, &mx, &my);

		if (result > -1)
		{
			nk_input_button(self->UI()->Context(), (nk_buttons)result, std::lround(mx), std::lround(my), nk_bool(action == GLFW_PRESS));
		}
	}

	void Mapper::MouseMotionEventCallback([[maybe_unused]]GLFWwindow* window, double x, double y)
	{
		auto* self = Mapper::GetInstance();

		nk_input_motion(self->UI()->Context(), std::lround(x), std::lround(y));
	}

	void Mapper::MouseScrollEventCallback([[maybe_unused]] GLFWwindow* window, double offX, double offY)
	{
		auto* self = Mapper::GetInstance();

		nk_input_scroll(self->UI()->Context(), nk_vec2(static_cast<float>(offX), static_cast<float>(offY)));
	}

	Mapper::~Mapper()
	{
		_configData.Reset();
		glfwTerminate();
	}

	bool Mapper::Configure()
	{
		_self = this;

		if (glfwInit() == GLFW_FALSE)
		{
			return false;
		}


#if defined(_DEBUG_) || defined(_RELWDEBUGSYM_)

		_rootDir = ROOT_DIR;
		_configFile = _rootDir / "config/config.ini";

#else
		_rootDir = std::filesystem::current_path();
		_configFile = _rootDir / "config/config.ini";

#endif // _DEBUG_

		/*
			Load the config.ini file.
		*/

		if (std::filesystem::exists(_configFile))
		{
			auto err = _configData.LoadFile(_configFile.string().c_str());

			if (err < 0)
			{
				return false;
			}
		}
		else {
			return false;
		}

		/*
			Read and set the window dimensions.
		*/

		auto width = _configData.GetLongValue("startup_display", "width", -1);
		auto height = _configData.GetLongValue("startup_display", "height", -1);

		if (width < 0 || height < 0)
		{
			return false;
		}

		/*
			If one of the dimensions is zero, it's safe to assume the native display resolution is being used.
		*/
		if (width == 0 || height == 0)
		{
			_window.SetSize(width, height);
			_fullscreen = true;
		}

		auto luaExceptionHandler = [](lua_State* L, sol::optional<const std::exception&> exception, sol::string_view description) -> int {

			std::string output = "[Lua Exception]: ";

			if(exception)
			{
				output += exception->what();
				std::puts(output.c_str());
			} else {
				output += description.data();
				std::puts(output.c_str());
			}

			return sol::stack::push(L, description);
		};

		_lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);
		_lua.set_exception_handler(luaExceptionHandler);

		// Window commands

		auto window = _lua.new_usertype<Window>("Window", 
			"GetWidth", &Window::GetWidth,
			"GetHeight", &Window::GetHeight
		);

		_lua["Win"] = &_window;

		window["Close"] = [this]() { glfwSetWindowShouldClose(_window.GetPtr(), 1); };

		window["Toggle"] = [this]() {
			
			if (IsFullscreen())
			{
				glfwRestoreWindow(_window.GetPtr());
				_window.RefreshSize();
			}
			else
			{
				glfwMaximizeWindow(_window.GetPtr());
				_window.RefreshSize();
			}
			
		};

		window["Restore"] = [this]() { glfwRestoreWindow(_window.GetPtr()); };

		window["Iconify"] = [this]() { glfwIconifyWindow(_window.GetPtr()); };

		

		return true;
	}


	int Mapper::Run()
	{
		using time = std::chrono::high_resolution_clock;

		if(!_window.Construct(_title, _fullscreen))
		{
			return 2;
		}

		glfwSetErrorCallback(Window::ContextErrorMessage);

		/*
			Create internal Renderer, UIContainer, and Scene objects.
		*/

		_renderer = std::make_unique<Renderer>(_rootDir.string().c_str());
		_renderer->SetRenderSize(_window.GetWidth(), _window.GetHeight());
		_renderer->SetViewport(0, 0, _window.GetWidth(), _window.GetHeight());
		_renderer->Init(Renderer::mode::Two);

		_ui = std::make_shared<UIContainer>(_fonts, _lua, _renderer.get());
		_ui->InitLua();

		if(!_ui->SetDefinitions(GetUIDir(), _lua))
		{
			return EXIT_FAILURE;
		}

		glfwSetKeyCallback(_window.GetPtr(), Mapper::KeyboardEventCallback);
		glfwSetCharCallback(_window.GetPtr(), Mapper::TextEventCallback);
		glfwSetMouseButtonCallback(_window.GetPtr(), Mapper::MouseButtonEventCallback);
		glfwSetCursorPosCallback(_window.GetPtr(), Mapper::MouseMotionEventCallback);
		glfwSetScrollCallback(_window.GetPtr(), Mapper::MouseScrollEventCallback);

		_scene = std::make_unique<Scene>(_ui);

		/*
			Show main window and start main loop.

			Note: I decided to go with ints for the color values because it was easy to compensate
				for erroneous data. Also the only floating-point type for INI is double, which would
				mean running into precision loss.
		*/

		static constexpr auto maxColorInt = 255;
		static constexpr auto maxColorFloat = 255.f;

		auto red = _configData.GetLongValue("startup_display", "bg_red", 0);
		auto green = _configData.GetLongValue("startup_display", "bg_green", 0);
		auto blue = _configData.GetLongValue("startup_display", "bg_blue", 0);

		if(red < 0) { red = -red; }
		if(green < 0) { green = -green; }
		if(blue < 0) { blue = -blue; }

		if(red > maxColorInt) { red %= (maxColorInt + 1); }
		if(green > maxColorInt) { green %= (maxColorInt + 1); }
		if(blue > maxColorInt) { blue %= (maxColorInt + 1); }

		glfwShowWindow(_window.GetPtr());
		Renderer::SetBackgroundColor(glm::vec4{ 
			static_cast<float>(red) / maxColorFloat, 
			static_cast<float>(green) / maxColorFloat,
			 static_cast<float>(blue) / maxColorFloat,
			  1.f 
			});

		const time::time_point last = time::now();

		while (glfwWindowShouldClose(_window.GetPtr()) == GLFW_FALSE && _appRunning)
		{
			const time::time_point current = time::now();
			const auto seconds = std::chrono::duration<float, std::chrono::seconds::period>(current - last);

			_scene->Update(seconds.count());

			_renderer->Begin();

			_renderer->End(_scene->GetUIDrawCalls());

			glfwSwapBuffers(_window.GetPtr());

			/*
				Capture input events for the GUI and the simulation.
			*/
			nk_input_begin(_ui->Context());
			glfwPollEvents();
			nk_input_end(_ui->Context());
		}

		if (_configUpdate)
		{
			_configData.SaveFile(_configFile.c_str());
		}

		return EXIT_SUCCESS;

	}

	void Mapper::SetContextSize(int w, int h)
	{
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

	int Mapper::GLFWKeytoNKKey(int key, int mods)
	{
		switch (key)
		{
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:			return NK_KEY_SHIFT;
		case GLFW_KEY_LEFT_CONTROL:
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
			if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
			{
				return NK_KEY_COPY;
			}
			
			break;
		}
		case GLFW_KEY_V:
		{
			if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
			{
				return NK_KEY_PASTE;
			}

			break;
		}
		case GLFW_KEY_X:
		{
			if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
			{
				return NK_KEY_CUT;
			}

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
