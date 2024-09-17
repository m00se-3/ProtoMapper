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
#include <memory>

namespace proto
{
	static constexpr int uiSystemID = 10;
	static constexpr int renSystemID = 12;

	Scene::Scene(Renderer* ren, std::shared_ptr<UIContainer> ui)
	{
		_systems.insert_or_assign(uiSystemID, ui);
		_systems.insert_or_assign(renSystemID, std::make_shared<RenderingSystem>(ren));
	}

	void Scene::Update(float dt)
	{
		// This is a conidate for improvement. dynamic_cast should be avoided.
		auto* ren = dynamic_cast<RenderingSystem*>(_systems.at(renSystemID).get());
		auto* ui = dynamic_cast<UIContainer*>(_systems.at(uiSystemID).get());

		if(ui->IsActive())
		{
			ui->Update(dt);
			auto calls = ui->Compile();
			ren->SetUIDrawCalls(calls);
		}

		ren->Update(dt);
	}

	void Scene::Cleanup()
	{
		
	}

	void Scene::Render()
	{

	}
}
