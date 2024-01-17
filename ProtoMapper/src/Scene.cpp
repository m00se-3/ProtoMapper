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

#include "RenderingSystem.hpp"
#include "UIContainer.hpp"

namespace proto
{
	Scene::Scene(const std::unordered_map<std::string, std::string>& data, Renderer* ren, Window* win)
	{
		systems.insert_or_assign("render", std::make_shared<RenderingSystem>(ren));
		auto ui = systems.insert_or_assign("ui", std::make_shared<UIContainer>(data.at("assets_dir"), win));

		static_cast<UIContainer*>(ui.first->second.get())->SetDefinitions(data.at("user_interface_dir"));
	}


	bool Scene::Init()
	{
		

		return true;
	}

	void Scene::Update(float dt)
	{
		auto& ren = systems.at("render");
		auto& ui = systems.at("ui");

		if (ren->IsActive() && ui->IsActive())
		{
			ui->Update(registry, dt);
			static_cast<RenderingSystem*>(ren.get())->SetUIDrawCalls(static_cast<UIContainer*>(ui.get())->Compile());
			ren->Update(registry, dt);
		}
	}

	void Scene::Cleanup()
	{
		
	}

	void Scene::Render()
	{

	}
}