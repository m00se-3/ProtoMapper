#ifndef PROTOMAPPER_SCENE_HPP
#define PROTOMAPPER_SCENE_HPP

#include "entt/entt.hpp"

#include <vector>
#include <list>


/*
	Defines the area occupied by the SceneNode. Any position values should be relative to this.
*/
struct Rectangle
{
	float x = 0.f, y = 0.f, w = 0.f, h = 0.f;

	Rectangle() = default;
	Rectangle(float x, float y, float w, float h);

	bool operator==(const Rectangle& other) const;

	bool Overlaps(const Rectangle& other) const;
	bool Contains(const Rectangle& other) const;
};

class Scene;	// Forward declaration.

/*
	A tree of SceneNodes is constructed to organize the draw calls, and to allow for Scene customization in-app.
*/
class SceneNode
{

protected:

	SceneNode* parent = nullptr;
	entt::entity ID;
	bool visible = false;
	struct Rectangle area;
	std::list<SceneNode*> children;

public:

	friend class Scene;

	SceneNode(SceneNode* par = nullptr);

	void Update(float dt);
	void Draw();
	void Destroy();
};

/*
	The main container class for the many maps and other components in the app.

	When maps are open they are added to the tree.
*/
class Scene
{
	
protected:
	SceneNode* root = nullptr;

public:
	Scene() = default;

	bool Init();
	void Update(float dt);
	void Draw();
	void Cleanup();

};

#endif // !PROTOMAPPER_SCENE_HPP
