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

#include <mutex>
#include <functional>

// Forward declarations.
class ResourceManager;
class Renderer;



class UIContainer
{

	std::mutex _mutex;
	/*
		All things below are necessary for nuklear to work and are, mostly, taken from the documentation.
	*/

	struct nk_context _ctx;
	struct nk_font_atlas _atlas;
	struct nk_font* _font;
	struct nk_convert_config _configurator;
	struct nk_buffer _cmds, _verts, _inds;
	Texture2D _fontTexture;

	unsigned int _vertexArray = 0u, _vb = 0u, _ib = 0u;
	void* _vertices = nullptr, * _indices = nullptr;

public:
	UIContainer();
	~UIContainer();

	void CompileUI();
	void DrawUI(Renderer* ren);

	void Lock();
	void Unlock();
	nk_context* Context();

	static void SetResourceManager(ResourceManager* ptr);


private:
	static ResourceManager* _resources;
	

};

#endif // !PROTOMAPPER_UICONTAINER_HPP
