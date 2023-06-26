#include "UIContainer.hpp"

#include "Vertex.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"

ResourceManager* UIContainer::_resources = nullptr;

constexpr long long MaxVertexBuffer = 4 * 1024;


static const struct nk_draw_vertex_layout_element vertex_layout[] = {
		{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Vertex2D, pos)},
		{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Vertex2D, texCoords)},
		{NK_VERTEX_COLOR, NK_FORMAT_R32G32B32A32_FLOAT, NK_OFFSETOF(struct Vertex2D, color)},
		{NK_VERTEX_LAYOUT_END}
};


void UIContainer::SetResourceManager(ResourceManager* ptr) { _resources = ptr; }

UIContainer::UIContainer()
{
	std::string fontFile = ROOT_DIR;
	fontFile += "/assets/fonts/keep_calm/KeepCalm-Medium.ttf";

	int imgWidth, imgHeight;
	_fontTexture = _resources->LoadTexture("KeepCalm-Medium");
	_fontTexture.Create();



	/*
		Initialize the UI library and the fonts.
	*/
	nk_font_atlas_init_default(&_atlas);
	nk_font_atlas_begin(&_atlas);

	_font = nk_font_atlas_add_from_file(&_atlas, fontFile.c_str(), 16, nullptr);

	const void* img = nk_font_atlas_bake(&_atlas, &imgWidth, &imgHeight, NK_FONT_ATLAS_RGBA32);
	_fontTexture.WriteData(img, imgWidth, imgHeight);
	nk_font_atlas_end(&_atlas, nk_handle_id(_fontTexture.ID), nullptr);

	if (!nk_init_default(&_ctx, &_font->handle)) return;

	_configurator.shape_AA = NK_ANTI_ALIASING_ON;
	_configurator.line_AA = NK_ANTI_ALIASING_ON;
	_configurator.vertex_layout = vertex_layout;
	_configurator.vertex_alignment = NK_ALIGNOF(Vertex2D);
	_configurator.vertex_size = sizeof(Vertex2D);
	_configurator.circle_segment_count = 20;
	_configurator.curve_segment_count = 20;
	_configurator.global_alpha = 1.0f;

	nk_buffer_init_default(&_cmds);

	/*
		Because I don't (yet) have the facilities to incorporate nukealr's buffer types into my own Buffer type,
		I have to set up the buffers manually.
	*/

	glGenVertexArrays(1, &_vertexArray);
	glGenBuffers(1, &_vb);
	glGenBuffers(1, &_ib);

	glBindVertexArray(_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, _vb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib);

	// Vertex positions
	glVertexAttribPointer(0u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), 0);
	glEnableVertexAttribArray(0u);

	// Texture coordinates
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2)));
	glEnableVertexAttribArray(1u);

	// Color values
	glVertexAttribPointer(2u, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2) + sizeof(glm::vec2)));
	glEnableVertexAttribArray(2u);

	glBindVertexArray(0);
}

UIContainer::~UIContainer()
{
	nk_font_atlas_cleanup(&_atlas);
	nk_font_atlas_clear(&_atlas);
	nk_free(&_ctx);

	_fontTexture.Reset();
	_resources->UnloadTexture("KeepCalm-Medium");
}

void UIContainer::Lock() { _mutex.lock(); }

void UIContainer::Unlock() { _mutex.unlock() }

void UIContainer::CompileUI()
{
	glBindVertexArray(_vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, _vb);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexBuffer, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxVertexBuffer, nullptr, GL_DYNAMIC_DRAW);

	_vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	_indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

	nk_buffer_init_fixed(&_verts, _vertices, MaxVertexBuffer);
	nk_buffer_init_fixed(&_inds, _indices, MaxVertexBuffer);

	nk_convert(&_ctx, &_cmds, &_verts, &_inds, &_configurator);

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glBindVertexArray(0u);
}

void UIContainer::DrawUI(Renderer* ren)
{
	/*
		Made using the examples in the nuklear repository. Made a rough API for nuklear to use with
		my Renderer class.
	*/

	const nk_draw_command* cmd = nullptr;

	const nk_draw_index* offset = nullptr;

	ren->UseTexture(_fontTexture);

	nk_draw_foreach(cmd, &_ctx, &_cmds)
	{
		if (!cmd->elem_count) continue;
		ren->DrawFromExternal<unsigned int>(_vertexArray, cmd->elem_count, GL_TRIANGLES, offset);
		offset += cmd->elem_count;
	}

	nk_clear(&_ctx);
	nk_buffer_clear(&_cmds);

}

nk_context* UIContainer::Context() { return &_ctx; }