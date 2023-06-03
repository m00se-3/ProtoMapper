#ifndef PROTOMAPPER_SCENE_HPP
#define PROTOMAPPER_SCENE_HPP

#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_STANDARD_IO

#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_UINT_DRAW_INDEX
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_FIXED_TYPES

// We have to explicitly undefine NK_IMPLEMENTATION here to avoid linking errors.
#undef NK_IMPLEMENTATION
#include "nuklear.h"
#include "entt/entt.hpp"

#include "Texture.hpp"
#include "Components.hpp"

#include <vector>
#include <list>


class Scene;	// Forward declaration.
class Renderer;

/*
	A tree of SceneNodes is constructed to organize the draw calls, and to allow for Scene customization in-app.
*/
class SceneNode
{

protected:

	SceneNode* parent = nullptr;
	entt::entity id;
	bool visible = false;
	struct Rectangle area;
	std::list<SceneNode*> children;

public:

	friend class Scene;

	SceneNode(SceneNode* par, entt::entity inID);

	entt::entity ID() const;
	const struct Rectangle& Area() const;
	void Update(Scene* container, float dt);
	void Draw();
	void Destroy();
	void SetArea(const struct Rectangle& newArea);
};

/*
	The main container class for the many maps and other components in the app.

	When maps are open they are added to the tree.
*/
class Scene
{

protected:
	SceneNode* root = nullptr;
	entt::registry manager;

	/*
		All things below are necessary for nuklear to work and are, mostly, taken from the documentation.
	*/

	struct nk_context ctx;
	struct nk_font_atlas atlas;
	struct nk_font* font;
	struct nk_convert_config configurator;
	struct nk_buffer cmds, verts, inds;
	Texture2D fontTexture;

	unsigned int vertexArray = 0u, vb = 0u, ib = 0u;

	void* vertices = nullptr, * indices = nullptr;

public:
	Scene() = default;

	bool Init();
	void Update(float dt);
	void DrawNodes();
	void CompileUI();
	void DrawUI(Renderer* ren);
	void Cleanup();

	nk_context* Context();
	entt::registry& Manager();

};

#endif // !PROTOMAPPER_SCENE_HPP
