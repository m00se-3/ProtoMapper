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
#include <print>
#include <span>
#include <unordered_map>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "stb_image.h"
#include "SimpleIni.h"

export module proto.Mapper;

import proto.UI.Container;
import proto.Scene;
import proto.Renderer;
import proto.ResourceManager;
import proto.Logger;

namespace proto 
{
	export class Mapper
	{
	public:
		Mapper() = default;
		~Mapper();

		static Mapper* GetInstance();

		[[nodiscard]]bool Configure();
		[[nodiscard]]int Run();
		void SetWindow(int w, int h);

		int GetWindowWidth() const;
		int GetWindowHeight() const;

		Renderer* GetRenderer();
		UIContainer* UI();

		static void DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

		/*
			GLFW callback functions
		*/

		static void ContextErrorMessage(int code, const char* description);
		static void MonitorCallback(GLFWmonitor* monitor, int event);
		static void KeyboardEventCallback(GLFWwindow* window, int keyn, int scancode, int action, int mods);
		static void TextEventCallback(GLFWwindow* window, unsigned int codepoint);
		static void MouseButtonEventCallback(GLFWwindow* window, int button, int action, int mods);
		static void MouseMotionEventCallback(GLFWwindow*, double x, double y);
		static void MouseScrollEventCallback(GLFWwindow* window, double offX, double offY);
		static void DropEventCallback(GLFWwindow* window, int count, const char** paths);
		static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);

		// static protoui::ButtonState GetButtonStateFromGLFWState(int state);

	private:
		const std::string _title = "ProtoMapper";
		std::filesystem::path _rootDir;
		bool _appRunning = true, _fullscreen = true, _configUpdate = false;

		int _wWidth = 1024, _wHeight = 768;

		GLFWwindow* _window = nullptr;
		GLFWmonitor* _monitor = nullptr;

		std::unique_ptr<uint8_t[]> _stringMemoryBuffer;

		std::unique_ptr<Scene> _scene;
		std::unique_ptr<UIContainer> _ui;
		std::unique_ptr<Renderer> _renderer;
		std::unique_ptr<ResourceManager> _resources;

		std::filesystem::path _configFile;
		CSimpleIniA _configData;

		// Text directories as defined in config.ini section [preload_directories].
		std::unordered_map<std::string, std::string> _dataTextFields;

		// Self pointer for use in the GLFW callbacks.
		static Mapper* _self;

	};

	/*
		Implementation.
	*/


	constexpr const size_t InitialTextBufferSize = 8u * 1024u;	// Allocate 8 KB for the text memory buffer. Can change later if needed.

	Mapper* Mapper::_self = nullptr;

	Mapper* Mapper::GetInstance() { return _self; }


	void Mapper::DebugOpenGL(GLenum src, GLenum type, [[maybe_unused]] GLuint id, GLenum severity, [[maybe_unused]] GLsizei length, const GLchar* message, [[maybe_unused]] const void* userParam)
	{
		std::println("Error {} {} {} - {}", src, type, severity, message);
	}

	void Mapper::ContextErrorMessage(int code, const char* description)
	{
		std::println("Error code {} - {}", code, description);
	}

	void Mapper::MonitorCallback([[maybe_unused]] GLFWmonitor* monitor, [[maybe_unused]] int event)
	{
		if (event == GLFW_CONNECTED)
		{

		}
		else if (event == GLFW_DISCONNECTED)
		{

		}
	}

	void Mapper::KeyboardEventCallback([[maybe_unused]] GLFWwindow* window, int keyn, int scancode, int action, int mods)
	{
		auto* self = Mapper::GetInstance()->UI()->InputHandle();

		auto key = Gwk::Input::GLFW3::KeyEvent{ keyn, scancode, action, mods };

		self->ProcessKeyEvent(key);
	}

	void Mapper::TextEventCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] unsigned int codepoint)
	{
	}

	void Mapper::MouseButtonEventCallback([[maybe_unused]] GLFWwindow* window, int button, int action, int mods)
	{
		Mapper::GetInstance()->UI()->InputHandle()->ProcessMouseButtons(button, action, mods);
	}

	void Mapper::MouseMotionEventCallback([[maybe_unused]]GLFWwindow*, double x, double y)
	{
		Mapper::GetInstance()->UI()->InputHandle()->ProcessMouseMove(x, y);
	}

	void Mapper::MouseScrollEventCallback([[maybe_unused]] GLFWwindow* window, double offX, double offY)
	{
		Mapper::GetInstance()->UI()->InputHandle()->ProcessScroll(offX, offY);
	}

	void Mapper::DropEventCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]]int count, [[maybe_unused]] const char** paths)
	{
	}

	void Mapper::FrameBufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height)
	{
		auto* self = Mapper::GetInstance();

		int oW = self->GetWindowWidth(), oH = self->GetWindowHeight();
		int rX = self->GetRenderer()->GetRenderX(),
			rY = self->GetRenderer()->GetRenderY(),
			rW = self->GetRenderer()->GetRenderWidth(),
			rH = self->GetRenderer()->GetRenderHeight();

		self->SetWindow(width, height);

		float sW = (float)(width / oW);
		float sH = (float)(height / oH);

		self->GetRenderer()->SetViewport(
			(int)((float)rX * sW),
			(int)((float)rY * sH),
			(int)((float)rW * sW),
			(int)((float)rH * sH)
		);
	}

	void Mapper::SetWindow(int w, int h)
	{
		_wWidth = w; _wHeight = h;
		_renderer->SetRenderSize(w, h);
		_ui->SetSize(w, h);
	}

	int Mapper::GetWindowWidth() const { return _wWidth; }
	
	int Mapper::GetWindowHeight() const { return _wHeight; }

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
			_wWidth = width;
			_wHeight = height;
			_fullscreen = false;
		}

		/*
			Setup the ResourceManager.
		*/

		_stringMemoryBuffer = std::make_unique<uint8_t[]>(InitialTextBufferSize);

		_resources = std::make_unique<ResourceManager>(std::span<uint8_t>{_stringMemoryBuffer.get(), InitialTextBufferSize});

		return true;
	}


	int Mapper::Run()
	{
		using time = std::chrono::high_resolution_clock;

		/*
			Create the window and the opengl context.
		*/

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
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
			glfwSwapInterval(1);

			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return 1;

			glDebugMessageCallback((GLDEBUGPROC)Mapper::DebugOpenGL, nullptr);

			glfwSetMonitorCallback(Mapper::MonitorCallback);
			glfwSetKeyCallback(_window, Mapper::KeyboardEventCallback);
			glfwSetCharCallback(_window, Mapper::TextEventCallback);
			glfwSetMouseButtonCallback(_window, Mapper::MouseButtonEventCallback);
			glfwSetCursorPosCallback(_window, Mapper::MouseMotionEventCallback);
			glfwSetScrollCallback(_window, Mapper::MouseScrollEventCallback);
			glfwSetDropCallback(_window, Mapper::DropEventCallback);
			glfwSetFramebufferSizeCallback(_window, Mapper::FrameBufferSizeCallback);

		}
		else
		{
			return 2;
		}

		/*
			Create internal Renderer, UIContainer, and Scene objects.
		*/

		_renderer = std::make_unique<Renderer>(_rootDir.string().c_str());
		_renderer->SetRenderSize(_wWidth, _wHeight);
		_renderer->SetViewport(0, 0, _wWidth, _wHeight);
		_renderer->Init(Renderer::mode::Two);

		auto paths = ProtoResourcePaths(_dataTextFields.at("assets_dir"));
		_ui = std::make_unique<UIContainer>(paths, _wWidth, _wHeight);

		Logger::Init(_ui->GetLogUI());

		_scene = std::make_unique<Scene>(_ui.get());
		_scene->Init();

		/*
			TODO: Load all assets on multiple threads.
		*/

		/*
			Load all preload assets and data files. 
		*/
		if(!_ui->SetDefinitionsPath(_dataTextFields.at("user_interface_dir"))) return 3;
		if(!_ui->ConstructWithProfile(_dataTextFields.at("profiles_dir") + _configData.GetValue("preferences", "profile"))) return 4;

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

		glfwShowWindow(_window);
		glClearColor(0.3f, 0.3f, 0.3f, 1.f);

		time::time_point last = time::now();

		while (!glfwWindowShouldClose(_window) && _appRunning)
		{
			time::time_point current = time::now();
			float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());

			_scene->Update(microseconds * 1000000.f);

			_renderer->Begin();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_scene->DrawNodes();
			_renderer->End();

			_ui->Draw();

			glfwSwapBuffers(_window);

			/*
				Capture input events for the GUI and the simulation.
			*/

			glfwPollEvents();
		}

		return EXIT_SUCCESS;

	}

	Renderer* Mapper::GetRenderer()
	{
		return _renderer.get();
	}

	UIContainer* Mapper::UI()
	{
		return _ui.get();
	}
}



