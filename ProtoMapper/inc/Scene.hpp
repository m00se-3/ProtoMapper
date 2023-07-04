/*
	ProtoMapper - Map creation and pathfinding software for game development.
	Copyright (C) 2023  Samuel Bridgham - moosethree473@gmail.com

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef PROTOMAPPER_SCENE_HPP
#define PROTOMAPPER_SCENE_HPP

#include "entt/entt.hpp"

#include "Texture.hpp"

#include <vector>
#include <list>


class Scene;	// Forward declaration.
class Renderer;
class UIContainer;

/*
	A tree of SceneNodes is constructed to organize the draw calls, and to allow for Scene customization in-app.
*/
class SceneNode
{

protected:

	SceneNode* parent = nullptr;
	entt::entity id;
	std::list<SceneNode*> children;

public:

	friend class Scene;

	SceneNode(SceneNode* par, entt::entity inID);

	entt::entity ID() const;
	void Update(Scene* container, float dt);
	void Draw();
	void Destroy();
};

/*
	The main container class for the many maps and other components in the app.

	When maps are open they are added to the tree.
*/
class Scene
{

	// Should be thread-safe.
	UIContainer* _uiInternal = nullptr;

protected:
	SceneNode* root = nullptr;
	entt::registry manager;


public:
	Scene(UIContainer* ptr);

	bool Init();
	void Update(float dt);
	void DrawNodes();
	void Cleanup();

	entt::registry& Manager();

	static void SetResourceManager(ResourceManager* ptr);

};

#endif // !PROTOMAPPER_SCENE_HPP
