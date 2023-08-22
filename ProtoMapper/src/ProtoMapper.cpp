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

#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>
#include <functional>

namespace proto
{

	static constexpr std::array<int, 6u> guiKeys = {
		GlobalState::Keys::right,
		GlobalState::Keys::left,
		GlobalState::Keys::down,
		GlobalState::Keys::up,
		GlobalState::Keys::back,
		GlobalState::Keys::enter
	};

	Mapper* Mapper::_self = nullptr;
	GlobalState Mapper::_internalState{};

	constexpr const size_t InitialTextBufferSize = 4u * 1024u;	// Allocate 4 KB for the text memory buffer. Can change later if needed.


	Mapper* Mapper::GetInstance() { return _self; }

	GlobalState* Mapper::GetState() { return &_internalState; }

	GlobalState::GlobalState()
	{
		for (auto key : guiKeys)
		{
			keyStates[Keys(key)] = ButtonState::up;
		}
	}

	void GlobalState::Reset()
	{
		mouseWheel = 0.0f;

		for (auto& key : keyStates)
		{
			if (key.second == ButtonState::released) key.second == ButtonState::up;
		}
	}


	void Mapper::DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam)
	{
		fprintf_s(stderr, "Error [%u] [%u] [%u] - %s", src, type, severity, message);
	}

	void Mapper::ContextErrorMessage(int code, const char* description)
	{
		fprintf_s(stderr, "Error code [%u] - %s\n", code, description);
	}

	void Mapper::MonitorCallback(GLFWmonitor* monitor, int event)
	{
		auto* self = Mapper::GetInstance();

		if (event == GLFW_CONNECTED)
		{

		}
		else if (event == GLFW_DISCONNECTED)
		{

		}
	}

	void Mapper::KeyboardEventCallback(GLFWwindow* window, int keyn, int scancode, int action, int mods)
	{
		auto* state = Mapper::GetState();

		if (state->keyStates.contains(GlobalState::Keys(keyn)))
		{
			switch (action)
			{
			case GLFW_PRESS:
			{
				auto& key = state->keyStates.at(GlobalState::Keys(keyn));
				key = ButtonState::pressed;
				break;
			}
			case GLFW_REPEAT:
			{
				auto& key = state->keyStates.at(GlobalState::Keys(keyn));
				key = ButtonState::down;
				break;
			}
			case GLFW_RELEASE:
			{
				auto& key = state->keyStates.at(GlobalState::Keys(keyn));
				key = ButtonState::released;
				break;
			}
			}
		}

	}

	void Mapper::TextEventCallback(GLFWwindow* window, unsigned int codepoint)
	{

	}

	void Mapper::MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods)
	{
		auto* state = Mapper::GetState();

		if (button < 3)
		{
			switch (action)
			{
			case GLFW_PRESS:
			{
				state->mouseButtons[button] = ButtonState::pressed;
				break;
			}
			case GLFW_REPEAT:
			{
				state->mouseButtons[button] = ButtonState::down;
				break;
			}
			case GLFW_RELEASE:
			{
				state->mouseButtons[button] = ButtonState::released;
				break;
			}
			}
		}

	}

	// This function exists mainly to pass the information to raygui.

	void Mapper::MouseMotionEventCallback(GLFWwindow*, double x, double y)
	{
		auto* state = Mapper::GetState();

		state->mouseX = static_cast<float>(x);
		state->mouseY = static_cast<float>(y);
	}

	void Mapper::MouseScrollEventCallback(GLFWwindow* window, double offX, double offY)
	{
		auto* state = Mapper::GetState();

		state->mouseWheel = static_cast<float>(offY);
	}

	void Mapper::DropEventCallback(GLFWwindow* window, int count, const char** paths)
	{
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

		delete[] _textMemoryBuffer;

		glfwDestroyWindow(_window);

		glfwTerminate();
	}

	bool Mapper::Configure()
	{
		_self = this;

		glfwSetErrorCallback(Mapper::ContextErrorMessage);

		if (!glfwInit())
		{
			return false;
		}

		/*
			Images are loaded the right way up.
		*/
		stbi_set_flip_vertically_on_load(1);


#if defined(_DEBUG_) || defined(_RELEASE_)

		_rootDir = ROOT_DIR;
		_configFile = _rootDir.string() + "/config/config.ini";
		_uiFile = _rootDir.string() + "/config/ui/data.ini";

#else
		_rootDir = ".";
		_configFile = _rootDir.string() + "/config/config.ini";
		_uiFile = _rootDir.string() + "/config/ui/data.ini";

#endif // _DEBUG_

		if (std::filesystem::exists(_configFile))
		{
			auto err = _configData.LoadFile(_configFile.string().c_str());

			if (err < 0) return false;
		}
		else
			return false;


		auto width = _configData.GetLongValue("startup_display", "width", -1);
		auto height = _configData.GetLongValue("startup_display", "height", -1);

		if (width < 0 || height < 0)
			return false;

		/*
			If one of the dimensions is zero, it's safe to assume the native display is being used.
		*/
		if (width != 0 && height != 0)
		{
			_wWidth = static_cast<unsigned int>(width);
			_wHeight = static_cast<unsigned int>(height);
			_fWidth = static_cast<float>(width);
			_fHeight = static_cast<float>(height);
			_fullscreen = false;
		}

		/*
			Set up the ResourceManager object and hook it up to the resource types it needs to track.
		*/

		_textMemoryBuffer = new char[InitialTextBufferSize];

		_resources = std::make_unique<ResourceManager>(_textMemoryBuffer, InitialTextBufferSize);

		Renderer::SetResourceManager(_resources.get());
		Texture2D::SetResourceManager(_resources->Textures());
		Shader::SetResourceManager(_resources->Shaders());
		UIContainer::SetResourceManager(_resources->Textures());

		return true;
	}


	void Mapper::Run()
	{
		using time = std::chrono::high_resolution_clock;

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		_monitor = glfwGetPrimaryMonitor();

		if (_fullscreen)
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, _title.c_str(), _monitor, nullptr);
		}
		else
		{
			_window = glfwCreateWindow(_wWidth, _wHeight, _title.c_str(), nullptr, nullptr);
		}

		if (_window)
		{
			glfwMakeContextCurrent(_window);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

			glDebugMessageCallback((GLDEBUGPROC)Mapper::DebugOpenGL, nullptr);

			glfwSetMonitorCallback(Mapper::MonitorCallback);
			glfwSetKeyCallback(_window, Mapper::KeyboardEventCallback);
			glfwSetCharCallback(_window, Mapper::TextEventCallback);
			glfwSetMouseButtonCallback(_window, Mapper::MouseButtonEventCallback);
			glfwSetCursorPosCallback(_window, Mapper::MouseMotionEventCallback);
			glfwSetScrollCallback(_window, Mapper::MouseScrollEventCallback);
			glfwSetDropCallback(_window, Mapper::DropEventCallback);
		}
		else
		{
			return;
		}

		_renderer = std::make_unique<Renderer>(_rootDir.string().c_str());

		_renderer->SetRenderWindow(_fWidth, _fHeight);
		_renderer->Init(Renderer::mode::Two);


		glClearColor(0.3f, 0.3f, 0.3f, 1.f);

		time::time_point last = time::now();

		_ui = std::make_unique<UIContainer>();

		if (!_ui->SetData(_uiFile))
		{
			fprintf_s(stderr, "Error: data.ini file not found...\n");
			return;
		}

		_scene = std::make_unique<Scene>(_ui.get());
		_scene->Init();

		glfwShowWindow(_window);

		while (!glfwWindowShouldClose(_window) && _appRunning)
		{
			time::time_point current = time::now();
			float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());

			_scene->Update(microseconds * 1000000.f);
			_ui->UpdateUI(_fWidth, _fHeight);

			_renderer->Begin();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_scene->DrawNodes();
			_ui->CompileUI();
			_ui->DrawUI(_renderer.get());

			_renderer->End();

			glfwSwapBuffers(_window);

			/*
				Capture input events for the GUI and the simulation.
			*/

			_internalState.Reset();
			glfwPollEvents();
		}

	}

	void Mapper::GetUILock() { _ui->Lock(); }

	void Mapper::ReleaseUILock() { _ui->Unlock(); }


	int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
	{
		Mapper mapper;

		if (mapper.Configure())
		{
			mapper.Run();
		}
		else
		{
			fprintf_s(stdout, "Failed to load application configurations. Please reinstall the program.");
		}

	}
}
