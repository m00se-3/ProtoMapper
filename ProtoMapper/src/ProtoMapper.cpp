#include "ProtoMapper.hpp"

#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Shader.hpp"

#undef STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <chrono>
#include <filesystem>


void ProtoMapper::DebugOpenGL(GLenum src, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]GLsizei length, const GLchar* message, [[maybe_unused]]const void* userParam)
{
	printf_s("Error [%u] [%u] [%u] - %s", src, type, severity, message);
}

void ProtoMapper::ContextErrorMessage([[maybe_unused]]int code, const char* description)
{
	printf_s("s", description);
}

void ProtoMapper::MonitorCallback(GLFWmonitor* monitor, int event)
{
	if (event == GLFW_CONNECTED)
	{

	}
	else if(event == GLFW_DISCONNECTED)
	{

	}
}

ProtoMapper::~ProtoMapper() 
{
	if (_configUpdate)
	{
		_configData.SaveFile(_configFile.c_str());
	}

	_configData.Reset();
	_scene.Cleanup();

	glfwDestroyWindow(_window);

	glfwTerminate();
}

bool ProtoMapper::Configure()
{
	glfwSetErrorCallback(ProtoMapper::ContextErrorMessage);

	if (!glfwInit())
	{
		return false;
	}

	/*
		Images are loaded the right way up.
	*/
	stbi_set_flip_vertically_on_load(1);


#if defined(_DEBUG_) || defined(_RELEASE_)

	_assetsDir = ASSETS_DIR;
	_configFile = _assetsDir + "/config/config.ini";

#else
	_assetsDir = "./assets"
	_configFile = _assetsDir + "/config/config.ini"

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

	_resources.reset();

	Renderer::SetResourceManager(_resources.get());
	Texture2D::SetResourceManager(_resources.get());
	Shader::SetResourceManager(_resources.get());
	Scene::SetResourceManager(_resources.get());

	return true;
}


void ProtoMapper::Run()
{
	using time = std::chrono::high_resolution_clock;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_monitor = glfwGetPrimaryMonitor();
	
	if (_fullscreen)
	{
		_window = glfwCreateWindow(_wWidth, _wHeight, std::string{_title + VERSION_NUMBER}.c_str(), _monitor, nullptr);
	}
	else
	{
		_window = glfwCreateWindow(_wWidth, _wHeight, std::string{_title + VERSION_NUMBER}.c_str(), nullptr, nullptr);
	}

	if (_window)
	{
		glfwMakeContextCurrent(_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return;

		glDebugMessageCallback((GLDEBUGPROC)ProtoMapper::DebugOpenGL, nullptr);
	}
	else
	{
		return;
	}

	_renderer.reset(new Renderer(_assetsDir));

	_renderer->SetRenderWindow(_fWidth, _fHeight);
	_renderer->Init(Renderer::mode::Two);


	glClearColor(0.f, 0.f, 0.f, 1.f);

	time::time_point last = time::now();

	_scene.Init();

	glfwShowWindow(_window);

	while (_appRunning)
	{
		time::time_point current = time::now();
		float microseconds = float(std::chrono::duration_cast<std::chrono::microseconds>(current - last).count());

		

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		_scene.Update(microseconds * 1000000.f);

		_scene.DrawNodes();
		_scene.CompileUI();
		_scene.DrawUI(_renderer.get());
		
		glfwSwapBuffers(_window);

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