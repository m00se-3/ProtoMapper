#ifndef PROTOMAPPER_UICONTAINER_HPP
#define PROTOMAPPER_UICONTAINER_HPP

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_FIXED_TYPES

// We have to explicitly undefine NK_IMPLEMENTATION here to avoid linking errors.
#undef NK_IMPLEMENTATION
#include "nuklear/nuklear.h"

#include "Texture.hpp"
#include "SimpleIni.h"

#include <mutex>
#include <map>
#include <string>
#include <filesystem>

// Forward declarations.
class Renderer;



class UIContainer
{
	// For thread-safe access.
	std::mutex _mutex;

	/*
		This holds the data that is customizable for the builtin uier interface.
	*/
	std::string _dataFile;
	CSimpleIniA _uiData;
	bool _updateUIData = false;

	/*
		All things below are necessary for nuklear to work and are, mostly, taken from the documentation.
	*/

	struct nk_context _ctx;
	struct nk_font_atlas _atlas;
	struct nk_font* _font;
	struct nk_convert_config _configurator;
	struct nk_buffer _cmds, _verts, _inds;
	struct nk_draw_null_texture _nullTexture;
	Texture2D _fontTexture;

	unsigned int _vertexArray = 0u, _vb = 0u, _ib = 0u;
	void* _vertices = nullptr, * _indices = nullptr;

	/*
		End nuklear buffer variables.
	*/

	// A map of window ids and titles, respectively.
	std::map<std::string, std::string> _windows;

public:
	UIContainer();
	~UIContainer();

	bool SetData(const std::filesystem::path& filepath);
	void UpdateUI(float wWidth, float wHeight); // This is the big function.
	void CompileUI();
	void DrawUI(Renderer* ren);

	void Lock();
	void Unlock();
	nk_context* Context();


	static void SetResourceManager(Texture2DManager* ptr);


private:
	static Texture2DManager* _texMan;
	

};

#endif // !PROTOMAPPER_UICONTAINER_HPP
