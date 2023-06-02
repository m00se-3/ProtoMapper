#include "Scene.hpp"

#include "Vertex.hpp"
#include "Renderer.hpp"

constexpr long long MaxVertexBuffer = 4 * 1024;


static const struct nk_draw_vertex_layout_element vertex_layout[] = {
		{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Vertex2D, pos)},
		{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Vertex2D, texCoords)},
		{NK_VERTEX_COLOR, NK_FORMAT_R32G32B32A32_FLOAT, NK_OFFSETOF(struct Vertex2D, color)},
		{NK_VERTEX_LAYOUT_END}
};

Rectangle::Rectangle(float x, float y, float w, float h)
	: x(x), y(y), w(w), h(h)
{
}

bool Rectangle::operator==(const Rectangle& other) const
{
	return (x == other.x && y == other.y && w == other.w && h == other.h);
}

bool Rectangle::Overlaps(const Rectangle& other) const
{
	return (
		x < other.x + other.w && y < other.y + other.h
		&& x + w > other.x && y + h > other.y
		);
}

bool Rectangle::Contains(const Rectangle& other) const
{
	return (
		x < other.x && x + w > other.x + other.w
		&& y < other.y && y + h > other.y + other.h
		);
}



bool Scene::Init()
{
	std::string fontFile = ASSETS_DIR;
	fontFile += "/fonts/keep_calm/KeepCalm-Medium.ttf";

	int imgWidth, imgHeight;
	fontTexture.Create();

	/*
		Initialize the UI library and the fonts.
	*/
	nk_font_atlas_init_default(&atlas);
	nk_font_atlas_begin(&atlas);

	font = nk_font_atlas_add_from_file(&atlas, fontFile.c_str(), 12, nullptr);

	const void* img = nk_font_atlas_bake(&atlas, &imgWidth, &imgHeight, NK_FONT_ATLAS_RGBA32);
	fontTexture.WriteData(img, imgWidth, imgHeight);
	nk_font_atlas_end(&atlas, nk_handle_id(fontTexture.ID), nullptr);
	nk_font_atlas_cleanup(&atlas);

	if (!nk_init_default(&ctx, &font->handle)) return false;

	configurator.shape_AA = NK_ANTI_ALIASING_ON;
	configurator.line_AA = NK_ANTI_ALIASING_ON;
	configurator.vertex_layout = vertex_layout;
	configurator.vertex_alignment = alignof(Vertex2D);
	configurator.vertex_size = sizeof(Vertex2D);
	configurator.circle_segment_count = 20;
	configurator.curve_segment_count = 20;
	configurator.global_alpha = 1.0f;

	nk_buffer_init_default(&cmds);

	/*
		Because I don't (yet) have the facilities to incorporate nukealr's buffer types into my own Buffer type,
		I have to set up the buffers manually.
	*/

	glGenVertexArrays(1, &vertexArray);
	glGenBuffers(1, &vb);
	glGenBuffers(1, &ib);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);

	// Vertex positions
	glVertexAttribPointer(0u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), 0);
	glEnableVertexAttribArray(0);

	// Texture coordinates
	glVertexAttribPointer(1u, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2)));
	glEnableVertexAttribArray(1);

	// Color values
	glVertexAttribPointer(2u, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (const void*)(sizeof(glm::vec2) + sizeof(glm::vec2)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(vertexArray);

	/*
		Create the root SceneNode, this will draw a background if there are no open files.
	*/

	auto rID = manager.create();

	root = new SceneNode(nullptr, rID);
	root->SetArea(Rectangle{ 50.f, 50.f, 200.f, 100.f });

	manager.emplace<UIElement>(rID, [](nk_context* context, const Rectangle& rect) {
		if (nk_begin(context, "Hello world!", nk_rect(rect.x, rect.y, rect.w, rect.h), 0))
		{
			nk_end(context);
		}
		});
	
	return true;
}

void Scene::Update(float dt)
{
	root->Update(this, dt);
}

void Scene::DrawNodes()
{
	root->Draw();
}

void Scene::CompileUI()
{
	glBindVertexArray(vertexArray);

	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, MaxVertexBuffer, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxVertexBuffer, nullptr, GL_DYNAMIC_DRAW);

	vertices = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	indices = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);

	nk_buffer_init_fixed(&verts, vertices, MaxVertexBuffer);
	nk_buffer_init_fixed(&inds, indices, MaxVertexBuffer);

	nk_convert(&ctx, &cmds, &verts, &inds, &configurator);

	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glBindVertexArray(0u);
}

void Scene::DrawUI(Renderer* ren)
{
	/*
		Made using the examples in the nuklear repository. Made a rough API for nuklear to use with
		my Renderer class.
	*/
	
	const nk_draw_command* cmd = nullptr;

	const nk_draw_index* offset = nullptr;

	nk_draw_foreach(cmd, &ctx, &cmds)
	{
		if (!cmd->elem_count) continue;
		ren->UseTexture(Texture2D{ (unsigned int)cmd->texture.id});
		ren->DrawFromExternal<unsigned int>(vertexArray, cmd->elem_count, GL_TRIANGLES, offset);
		offset += cmd->elem_count;
	}

	nk_clear(&ctx);
	nk_buffer_clear(&cmds);
}

void Scene::Cleanup()
{
	root->Destroy();
	delete root;

	nk_font_atlas_clear(&atlas);
	nk_free(&ctx);
}

nk_context* Scene::Context() { return &ctx; }

entt::registry& Scene::Manager() { return manager; }

SceneNode::SceneNode(SceneNode* par, entt::entity inID)
	: parent(par), id(inID)
{

}

entt::entity SceneNode::ID() const { return id; }

const struct Rectangle& SceneNode::Area() const { return area; }

void SceneNode::SetArea(const struct Rectangle& newArea)
{
	area = newArea;
}

void SceneNode::Update(Scene* container, float dt)
{
	auto& man = container->Manager();
	
	if (man.any_of<UIElement>(id))
	{
		auto& ui = man.get<UIElement>(id);
		ui.func(container->Context(), area);
	}
	else
	{
		// Do other kinds of updates.
	}

	for (auto& child : children)
	{
		child->Update(container, dt);
	}
}

void SceneNode::Draw()
{
}

void SceneNode::Destroy()
{
	for (auto* child : children)
	{
		child->Destroy();
		delete child;
	}
}
