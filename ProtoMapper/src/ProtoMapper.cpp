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

#include "GLFW/glfw3.h"
#include "SimpleIni.h"

export module proto.Mapper;

import proto.UI.Container;
import proto.Scene;
import proto.Renderer;
import proto.ResourceManager;
import proto.Logger;
import proto.Window;

namespace proto 
{
	export class Mapper
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

		// This is the only GLFW callback we need in this class, the rest are under the Window class.
		static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);

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

	/*
		Implementation.
	*/

	constexpr const size_t InitialTextBufferSize = 8u * 1024u;	// Allocate 8 KB for the text memory buffer. Can change later if needed.

	Mapper* Mapper::_self = nullptr;

	Mapper* Mapper::GetInstance() { return _self; }

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
		self->UI()->RenderViewport(nx, ny, nw, nh);

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

		auto paths = ProtoResourcePaths(_dataTextFields.at("assets_dir"));
		_ui = std::make_unique<UIContainer>(paths, _window.GetWidth(), _window.GetHeight());

		Window::SetUIHandle(_ui.get());
		Logger::Init(_ui->GetLogUI());

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
		if(!_ui->ConstructWithProfile(_dataTextFields.at("profiles_dir") + _configData.GetValue("preferences", "profile"), _window.GetPtr())) return 4;

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

			_renderer->Begin();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			_scene->DrawNodes();
			_renderer->End();

			_ui->Draw();

			glfwSwapBuffers(_window.GetPtr());

			/*
				Capture input events for the GUI and the simulation.
			*/

			glfwPollEvents();
		}

		return EXIT_SUCCESS;

	}

	void Mapper::SetContextSize(int w, int h)
	{
		_window.SetSize(w, h);
		_renderer->SetRenderSize(w, h);
		_ui->SetSize(w, h);
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
}



