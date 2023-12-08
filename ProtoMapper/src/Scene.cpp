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
#include "Scene.hpp"

namespace proto
{
	Scene::Scene(UIContainer* ptr)
		: _uiInternal(ptr)
	{

	}


	bool Scene::Init()
	{
		/*
			Create the root SceneNode, this will draw a background if there are no open files.
		*/

		auto rID = manager.create();

		root = new SceneNode(nullptr, rID);



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

	entt::registry& Scene::Manager() { return manager; }

	SceneNode::SceneNode(SceneNode* par, entt::entity inID)
		: parent(par), id(inID)
	{

	}

	entt::entity SceneNode::ID() const { return id; }

	void SceneNode::Update(Scene* container, float dt)
	{
		auto& man = container->Manager();


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