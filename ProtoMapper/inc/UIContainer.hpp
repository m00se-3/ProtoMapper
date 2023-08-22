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

#ifndef PROTOMAPPER_UICONTAINER_HPP
#define PROTOMAPPER_UICONTAINER_HPP

#include "Texture.hpp"
#include "SimpleIni.h"

#include <mutex>
#include <map>
#include <string>
#include <filesystem>

// Forward declarations.
class Renderer;



class UIContainer
{
	// For thread-safe access.
	std::mutex _mutex;

	/*
		This holds the data that is customizable for the builtin uier interface.
	*/
	std::string _dataFile;
	CSimpleIniA _uiData;
	bool _updateUIData = false;

	Texture2D _fontTexture;


public:
	UIContainer();
	~UIContainer();

	bool SetData(const std::filesystem::path& filepath);
	void UpdateUI(float wWidth, float wHeight); // This is the big function.
	void CompileUI();
	void DrawUI(Renderer* ren);

	void Lock();
	void Unlock();


	static void SetResourceManager(Texture2DManager* ptr);


private:
	static Texture2DManager* _texMan;
	

};

#endif // !PROTOMAPPER_UICONTAINER_HPP
