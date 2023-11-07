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
module;

#include "components/All.hpp"
#include "entt/entt.hpp"


#include <vector>
#include <list>

export module Scene;

import UIContainer;

export namespace proto
{

	class Scene;	// Forward declaration.
	class Renderer;
	class UIContainer;

	/*
		A tree of SceneNodes is constructed to organize the draw calls, and to allow for Scene customization in-app.
	*/
	export class SceneNode
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
	export class Scene
	{

		UIContainer* _uiInternal = nullptr;

	protected:
		SceneNode* root = nullptr;
		//entt::registry manager;


	public:
		Scene(UIContainer* ptr);

		bool Init();
		void Update(float dt);
		void DrawNodes();
		void Cleanup();

		//entt::registry& Manager();

	};

	Scene::Scene(UIContainer* ptr)
		: _uiInternal(ptr)
	{

	}


	bool Scene::Init()
	{
		/*
			Create the root SceneNode, this will draw a background if there are no open files.
		*/

		// auto rID = manager.create();

		// root = new SceneNode(nullptr, rID);



		return true;
	}

	void Scene::Update(float dt)
	{
		//root->Update(this, dt);
	}

	void Scene::DrawNodes()
	{
		//root->Draw();
	}



	void Scene::Cleanup()
	{
		//root->Destroy();
		//delete root;
	}

	//entt::registry& Scene::Manager() { return manager; }

	SceneNode::SceneNode(SceneNode* par, entt::entity inID)
		: parent(par), id(inID)
	{

	}

	entt::entity SceneNode::ID() const { return id; }

	void SceneNode::Update(Scene* container, float dt)
	{
		//auto& man = container->Manager();


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
}