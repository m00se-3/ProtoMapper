#include "Scene.hpp"

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
	
	/*
		Create the root SceneNode, this will draw a background if there are no open files.
	*/


	
	return true;
}

void Scene::Update(float dt)
{
}

void Scene::DrawNodes()
{
}

void Scene::DrawUI()
{
}

void Scene::Cleanup()
{
	root->Destroy();
	delete root;

	nk_font_atlas_clear(&atlas);
	nk_clear(&ctx);
}

SceneNode::SceneNode(SceneNode* par)
	: parent(par)
{

}

void SceneNode::Update(float dt)
{
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
