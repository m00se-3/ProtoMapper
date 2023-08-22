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

#include "UIContainer.hpp"

#include "Vertex.hpp"
#include "ResourceManager.hpp"
#include "Renderer.hpp"

Texture2DManager* UIContainer::_texMan = nullptr;


void UIContainer::SetResourceManager(Texture2DManager* ptr) { _texMan = ptr; }

UIContainer::UIContainer()
{
	std::string fontFile = ROOT_DIR;
	fontFile += "/assets/fonts/keep_calm/KeepCalm-Medium.ttf";

	int imgWidth, imgHeight;
	_fontTexture = _texMan->Load("KeepCalm-Medium");
	_fontTexture.Create();
	auto nTexture = _texMan->Get("default");


}

UIContainer::~UIContainer()
{
	_fontTexture.Reset();
	_texMan->Unload("KeepCalm-Medium");

	if (_updateUIData)
	{
		_uiData.SaveFile(_dataFile.c_str());
	}

	_uiData.Reset();
}

bool UIContainer::SetData(const std::filesystem::path& filepath)
{
	if (std::filesystem::exists(filepath) && filepath.extension() == ".ini")
	{
		_dataFile = filepath.string();
		_uiData.LoadFile(_dataFile.c_str());

		return true;
	}

	return false;
}

void UIContainer::Lock() { _mutex.lock(); }

void UIContainer::Unlock() { _mutex.unlock(); }

void UIContainer::CompileUI()
{
	
}

void UIContainer::DrawUI(Renderer* ren)
{
	

}

void UIContainer::UpdateUI(float wWidth, float wHeight)
{

	

}
