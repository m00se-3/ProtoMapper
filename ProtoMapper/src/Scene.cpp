#include "Scene.hpp"

#include "Vertex.hpp"


static const struct nk_draw_vertex_layout_element vertex_layout[] = {
		{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Vertex2D, pos.x)},
		{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(struct Vertex2D, texCoords.x)},
		{NK_VERTEX_COLOR, NK_FORMAT_R32G32B32A32_FLOAT, NK_OFFSETOF(struct Vertex2D, color.x)},
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
	nk_font_atlas_end(&atlas, nk_handle_id(fontTexture.ID()), nullptr);
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
	nk_buffer_init_default(&verts);
	nk_buffer_init_default(&inds);

	/*
		Create the root SceneNode, this will draw a background if there are no open files.
	*/

	auto rID = manager.create();

	root = new SceneNode(nullptr, rID);
	root->SetArea(Rectangle{ 50.f, 50.f, 800.f, 600.f });

	manager.emplace<UIElement>(rID, [](nk_context* context, const Rectangle& rect) {
		if (nk_begin(context, "Hello world!", nk_rect(rect.x + 0.5f * rect.x, rect.y + 0.5f * rect.y, 0.2f * rect.w, 0.4f * rect.h), 0))
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
	nk_convert(&ctx, &cmds, &verts, &inds, &configurator);
}

void Scene::DrawUI()
{
	/*
		Made using the examples in the nuklear repository. OpenGl must be called directly here because
		there's no way to know what texture a particular command is wanting. Also the offset variable
		is not used in my cheap renderer class.
	*/
	
	const nk_draw_command* cmd = nullptr;

	const nk_draw_index* offset = nullptr;

	nk_draw_foreach(cmd, &ctx, &cmds)
	{
		if (!cmd->elem_count) continue;
		glBindTexture(GL_TEXTURE_2D, cmd->texture.id);
		glDrawElements(GL_TRIANGLES, cmd->elem_count, GL_UNSIGNED_INT, offset);
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
