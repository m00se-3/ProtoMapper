#include "ProtoMapper.hpp"
#include "HeightMap2D.hpp"
#include "TopMenu.hpp"

#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "glad/glad.h"

#include <chrono>
#include <filesystem>

bool ProtoMapper::Update(float dt)
{

	return true;
}

bool ProtoMapper::Draw()
{
	if (_mapOpen)
	{
		

	}

	return true;
}


ProtoMapper::ProtoMapper()
	: _currentMap()
{
	
}

ProtoMapper::~ProtoMapper() 
{
	if (_configUpdate)
	{
		_configData.SaveFile(_configFile.c_str());
	}

	_configData.Reset();
	_currentMap.reset();

	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool ProtoMapper::Configure()
{
#if defined(_DEBUG_) || defined(_RELEASE_)

	_configFile = ASSETS_DIR;
	_configFile += "/assets/config/config.ini";

#else

	_configFile = "./assets/config/config.ini"

#endif // _DEBUG_

		if (std::filesystem::exists(_configFile))
		{
			auto err = _configData.LoadFile(_configFile.c_str());

			if (err < 0) return false;
		}
		else
			return false;


	auto width = _configData.GetLongValue("display", "width", -1);
	auto height = _configData.GetLongValue("display", "height", -1);

	if (width < 0 || height < 0)
		return false;

	/*
		If one of the dimensions is zero, it's safe to assume the native display is being used.
	*/
	if (width != 0)
	{
		_wWidth = static_cast<unsigned int>(width);
		_wHeight = static_cast<unsigned int>(height);
		_fullscreen = false;
	}

	if (SDL_Init(SDL_INIT_VIDEO) != 0 || IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG || TTF_Init() != 0)
	{
		SDL_Log("%s", SDL_GetError());
		return false;
	}

	return true;
}


void ProtoMapper::Run()
{
	using time = std::chrono::high_resolution_clock;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	
	if (_fullscreen)
	{
		_window = SDL_CreateWindow(std::string{_title + VERSION_NUMBER}.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _wWidth, _wHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED);
	}
	else
	{
		_window = SDL_CreateWindow(std::string{ _title + VERSION_NUMBER }.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, _wWidth, _wHeight, SDL_WINDOW_OPENGL);
	}

	if (_window)
	{
		_context = SDL_GL_CreateContext(_window);

		SDL_GL_MakeCurrent(_window, _context);

		if (!gladLoadGLLoader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress))) return;
	}
	else
	{
		SDL_Log("%s", SDL_GetError());
		return;
	}

	glClearColor(0.f, 0.f, 0.f, 1.f);

	tgui::GuiSDL _rootGui{ _window };
	
	TopMenu bar(this, _fWidth * 1.f, _fHeight * 0.05f);
	_rootGui.add(bar.GetGroup());

	time::time_point last = time::now();

	_currentMap->Generate(last.time_since_epoch().count());

	while (_appRunning)
	{
		SDL_Event event;

		time::time_point current = time::now();
		float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());

		while (SDL_PollEvent(&event))
		{
			if (_rootGui.handleEvent(event)) continue;
			
			switch (event.type)
			{
			case SDL_QUIT:
			{
				_appRunning = false;
				break;
			}
			case SDL_MOUSEBUTTONDOWN:
			{
				if (event.button.button == SDL_BUTTON_MIDDLE) _panning = true;
				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (event.button.button == SDL_BUTTON_MIDDLE) _panning = false;
				break;
			}
			case SDL_MOUSEWHEEL:
			{

				break;
			}
			case SDL_MOUSEMOTION:
			{

				break;
			}
			case SDL_WINDOWEVENT_RESIZED:
			{
				_wWidth = event.window.data1;
				_wHeight = event.window.data2;

				auto err1 = _configData.SetLongValue("display", "width", event.window.data1);
				auto err2 = _configData.SetLongValue("display", "height", event.window.data2);

				if (err1 != SI_UPDATED || err2 != SI_UPDATED)
				{
					fprintf_s(stdout, "New display values failed to save. Possibly a corrupt config file.");
					return;
				}

				_configUpdate = true;

				break;
			}
			}
		}

		if (!Update(microseconds * 1000000.f)) break;

		glClear(GL_COLOR_BUFFER_BIT);

		if (!Draw()) break;

		_rootGui.draw();
		
		SDL_GL_SwapWindow(_window);

	}

}


int main([[maybe_unused]]int argc, [[maybe_unused]]char** argv)
{
	ProtoMapper mapper;

	if (mapper.Configure())
	{
		mapper.Run();
	}
	else
	{
		fprintf_s(stdout, "Failed to load application configurations. Please reinstall the program.");
	}

	return 0; // Don't forget to return 0 from an SDL application. :$
}